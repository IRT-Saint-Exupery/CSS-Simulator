import subprocess
import threading
import time as pytime
import os
import paramiko
import sys
import re
import yaml

#================= Configuration =================

remote_machines = {
    '192.168.100.11': [
        '/tmp/logids_p0.csv',
        '/tmp/logids_p1.csv',
        '/tmp/logids_p2.csv',
        '/tmp/logids_p3.csv'
    ],
    # Add other machines if needed
}

destination_machine = {
    'ip': '192.168.100.5',
    'username': 'nos3',
    'password': 'nos3123!',
    'base_dir': "/tmp/scenario_output"
}

ssh_username = 'nos3'  # for remote probes
ssh_password = 'nos3123!'

DEFAULT_OUTPUT_DIR = destination_machine['base_dir']

#================= Helpers =================

def run_at(delay, func):
    t = threading.Timer(delay, func)
    t.start()
    return t

def create_ssh_client(host, username, password):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(hostname=host, username=username, password=password, timeout=10)
    return ssh

def sftp_mkdir_recursive(sftp, remote_directory):
    dirs = remote_directory.strip('/').split('/')
    path = ''
    for d in dirs:
        path += '/' + d
        try:
            sftp.stat(path)
        except FileNotFoundError:
            sftp.mkdir(path)

def get_sat_id_from_ip(ip):
    # Extract last number in IP and subtract 10
    try:
        last_octet = int(ip.strip().split('.')[-1])
        sat_id = last_octet - 10
        return max(sat_id, 0)  # Ensure non-negative
    except Exception:
        return 0

#================= YAML loading =================

def load_scenario_yaml(yaml_path):
    with open(yaml_path, "r") as f:
        cfg = yaml.safe_load(f)

    scenario_name = cfg.get("scenario_id")
    mission_id = cfg.get("mission_id")
    end_time = cfg.get("end_time")
    nb_satellites = cfg.get("nb_satellites")
    split = cfg.get("split")

    if not scenario_name or not mission_id or not end_time or not nb_satellites or not split:
        raise ValueError("scenario_id, mission_id, end_time, nb_satellites, and split are required in YAML")

    output_dir = cfg.get("output_dir") or DEFAULT_OUTPUT_DIR
    attacks = cfg.get("attacks", [])

    commands = []
    times = []
    for atk in attacks:
        cmd = atk.get("command_to_launch")
        t = atk.get("time")
        if cmd is None or t is None:
            continue
        commands.append(str(cmd))
        times.append(float(t))

    command_times = [int(t) for t in times]
    time_end = int(end_time)

    source_txt_path = os.path.join("/home/nos3/Scenario_Manager","mission_targets", f"{mission_id}.txt")
    remote_txt_path = os.path.join("/home/nos3/eclipse-workspace", "mission", "config", "targets.txt")

    if os.path.isfile(source_txt_path):
        try:
            with create_ssh_client(destination_machine['ip'], destination_machine['username'], destination_machine['password']) as ssh:
                sftp = ssh.open_sftp()
                remote_dir = os.path.dirname(remote_txt_path)
                sftp_mkdir_recursive(sftp, remote_dir)
                sftp.put(source_txt_path, remote_txt_path)
                sftp.close()
            print(f"[INFO] Replaced remote '{remote_txt_path}' with local '{source_txt_path}'")
        except Exception as e:
            print(f"[ERROR] Error replacing remote targets.txt file: {e}")
    else:
        print(f"[WARN] Source file {source_txt_path} does not exist. Skipping remote replacement.")

    return {
        "scenario_name": scenario_name,
        "mission_id": mission_id,
        "command_times": command_times,
        "commands": commands,
        "time_end": time_end,
        "split": split,
        "output_dir": output_dir,
    }

#================= PCAP helpers =================

def get_remote_pcap_path(output_dir, split, mission_id):
    return os.path.join(output_dir, "css-pcap-ds", "data", split, f"{mission_id}_ground_station.pcap")

def start_remote_tcpdump(ip_address, username, password, remote_pcap_path):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        ssh.connect(ip_address, username=username, password=password, timeout=10)
        remote_dir = os.path.dirname(remote_pcap_path)
        ssh.exec_command(f"mkdir -p {remote_dir}")
        cmd = f"sudo nohup tcpdump -i eth0 udp -w {remote_pcap_path} > /dev/null 2>&1 &"
        ssh.exec_command(cmd)
        print(f"[INFO] tcpdump started on {ip_address}, output file: {remote_pcap_path}")
        return ssh
    except Exception as e:
        print(f"[ERROR] Failed to start tcpdump on {ip_address}: {e}")
        ssh.close()
        return None

def stop_remote_tcpdump(ssh):
    if ssh is None:
        print("[WARN] SSH client not active. Skipping tcpdump stop.")
        return
    try:
        ssh.exec_command("sudo pkill tcpdump")
        print("[INFO] tcpdump stopped on remote machine")
    except Exception as e:
        print(f"[ERROR] Failed to stop tcpdump: {e}")
    finally:
        ssh.close()

#================= Attack launcher =================

def launch_cmd(cmd):
    try:
        subprocess.run(
            ["./Master_Attacks_Scripts.sh", str(cmd)],
            cwd="/home/nos3/CSS_Attacks",
            check=True,
        )
        print(f"[INFO] Attack command executed: {cmd}")
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Failed running command {cmd}: {e}")

#================= Logs dataset helpers =================

def extract_probe_from_filename(filename):
    base = os.path.basename(filename)
    probe_match = re.search(r'p[0-9]+', base)
    return probe_match.group(0) if probe_match else "p0"

#================= Copy logs from remote probes to destination =================

def copy_logs_to_destination(output_dir, split):
    try:
        dest_ssh = create_ssh_client(destination_machine['ip'], destination_machine['username'], destination_machine['password'])
        dest_sftp = dest_ssh.open_sftp()
    except Exception as e:
        print(f"[ERROR] Could not connect to destination machine for logs: {e}")
        return

    for ip, file_list in remote_machines.items():
        print(f"[INFO] Connecting to probe machine {ip} for logs...")
        try:
            ssh = create_ssh_client(ip, ssh_username, ssh_password)
            sftp = ssh.open_sftp()
            sat_id = get_sat_id_from_ip(ip)  # Calculate satid from IP once per machine
            for remote_path in file_list:
                try:
                    log_basename = os.path.basename(remote_path)
                    probe_id = extract_probe_from_filename(log_basename)
                    local_temp = f"/tmp/{log_basename}"

                    print(f"[INFO] Downloading log {remote_path} from {ip} to local temp {local_temp} ...")
                    if sat_id != 1:
                    	sftp.get(remote_path, local_temp)

                    if os.path.getsize(local_temp) == 0:
                        print(f"[WARN] Log file {remote_path} from {ip} is empty, skipping upload.")
                        #os.remove(local_temp)
                        continue

                    new_log_name = f"logids_{probe_id}_sat{sat_id}.csv"
                    remote_log_dest_dir = os.path.join(output_dir, "css-logs-ds", "data", probe_id, split)
                    remote_log_dest_path = os.path.join(remote_log_dest_dir, new_log_name)

                    try:
                        sftp_mkdir_recursive(dest_sftp, remote_log_dest_dir)
                    except Exception:
                        dest_ssh.exec_command(f"mkdir -p {remote_log_dest_dir}")

                    dest_sftp.put(local_temp, remote_log_dest_path)
                    print(f"[INFO] Uploaded log to destination machine: {remote_log_dest_path}")

                    os.remove(local_temp)
                except FileNotFoundError:
                    print(f"[WARN] Log file {remote_path} not found on probe {ip}, skipping.")
                except Exception as e:
                    print(f"[ERROR] Failed transferring log {remote_path} from {ip}: {e}")

            sftp.close()
            ssh.close()
        except Exception as e:
            print(f"[ERROR] Could not connect or retrieve logs from {ip}: {e}")

    dest_sftp.close()
    dest_ssh.close()

#================= Zip output_dir remotely and transfer back =================

def zip_and_transfer_output_dir(output_dir):
    zip_name = "scenario_output.zip"
    remote_zip_path = os.path.join(destination_machine['base_dir'], zip_name)
    local_zip_path = os.path.join("/tmp", zip_name)

    ssh = None
    try:
        ssh = create_ssh_client(destination_machine['ip'], destination_machine['username'], destination_machine['password'])
        base_dir = os.path.dirname(output_dir)
        target_dir = os.path.basename(output_dir)
        print(f"[INFO] Zipping directory {output_dir} remotely on {destination_machine['ip']} into {remote_zip_path} ...")
        
        # Ensure zip is installed and use -r to zip target directory inside base_dir
        cmd_zip = f"cd {base_dir} && zip -r {remote_zip_path} {target_dir}"
        stdin, stdout, stderr = ssh.exec_command(cmd_zip)
        exit_status = stdout.channel.recv_exit_status()
        if exit_status != 0:
            err = stderr.read().decode()
            print(f"[ERROR] Remote zip command failed: {err}")
            ssh.close()
            return

        print("[INFO] Remote zip complete. Starting transfer to local machine...")
        sftp = ssh.open_sftp()
        sftp.get(remote_zip_path, local_zip_path)
        print(f"[INFO] Zip file downloaded to local machine at: {local_zip_path}")

        print("[INFO] Cleaning up remote zip archive...")
        ssh.exec_command(f"rm -f {remote_zip_path}")

        sftp.close()
        ssh.close()
    except Exception as e:
        if ssh:
            ssh.close()
        print(f"[ERROR] Failed during remote zip and transfer: {e}")

#================= Main =================

def main():
    if len(sys.argv) != 2:
        print(f"Usage: python3 {sys.argv[0]} <scenario_yaml>")
        sys.exit(1)

    yaml_path = sys.argv[1]
    cfg = load_scenario_yaml(yaml_path)

    scenario_name = cfg["scenario_name"]
    mission_id = cfg["mission_id"]
    command_times = cfg["command_times"]
    commands = cfg["commands"]
    time_end = cfg["time_end"]
    split = cfg["split"]
    output_dir = cfg["output_dir"]

    if any(command_times[i] >= command_times[i + 1] for i in range(len(command_times) - 1)):
        raise ValueError("Attack times must be strictly ascending in YAML.")

    print(f"[INFO] Starting scenario: {scenario_name}")
    subprocess.run(['bash', 'start.sh'], cwd="/home/nos3/Desktop/github-nos3", check=True)
    print("[INFO] Mission environment initialized.")

    remote_pcap_path = get_remote_pcap_path(output_dir, split, mission_id)
    ssh_client_tcpdump = start_remote_tcpdump(
        destination_machine['ip'],
        destination_machine['username'],
        destination_machine['password'],
        remote_pcap_path,
    )

    for t, cmd in zip(command_times, commands):
        run_at(t, lambda c=cmd: launch_cmd(c))
    print("[INFO] Attack commands scheduled.")

    def end_scenario():
        #pytime.sleep(5)
        print("[INFO] Stopping tcpdump on destination machine...")
        stop_remote_tcpdump(ssh_client_tcpdump)

        #print("[INFO] Zipping output directory remotely and transferring back to local machine...")
        #zip_and_transfer_output_dir(output_dir) #TODO
        
        print("[INFO] Running mission environment cleanup script...")
        
        #pytime.sleep(25)
        subprocess.run(['bash', 'stopT.sh'], cwd="/home/nos3/Desktop/github-nos3", check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        pytime.sleep(3)
        
        print("[INFO] Ending scenario...")
        print("[INFO] Copying logs from probes to destination machine...")
        copy_logs_to_destination(output_dir, split)

    run_at(time_end, end_scenario)

if __name__ == "__main__":
    main()

