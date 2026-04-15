import json
import subprocess
import threading
import time as pytime
import os
import paramiko
import sys

if len(sys.argv) != 2:
    print(f"Usage: python {sys.argv[0]} <port>")
    sys.exit(1)

scenario = sys.argv[1]

def run_at(delay, func):
    """Run a function after a given delay (seconds) in a separate thread."""
    timer = threading.Timer(delay, func)
    timer.start()
    return timer

def start_remote_tcpdump(ip_address, username, password, data_output_name, remote_dir="/tmp"):
    """Start tcpdump remotely via SSH and return SSH client for later use."""
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        ssh.connect(ip_address, username=username, password=password, timeout=10)
        remote_path = os.path.join(remote_dir, data_output_name)
        command = f"sudo nohup tcpdump -i eth0 udp -w {remote_path} > /dev/null 2>&1 &"
        ssh.exec_command(command)
        print(f"[INFO] tcpdump started on {ip_address}, output: {remote_path}")
        return ssh  # Keep SSH session to stop later if needed
    except Exception as e:
        print(f"[ERROR] Failed to start tcpdump on {ip_address}: {e}")
        ssh.close()
        return None

def stop_remote_tcpdump(ssh):
    """Stop all tcpdump processes remotely via SSH."""
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

def launch_cmd(cmd):
    """Execute a single bash command with arguments in the specified directory."""
    try:
        subprocess.run(["./Master_Attacks_Scripts.sh", cmd], cwd="/home/nos3/CSS_Attacks", check=True)
        print(f"[INFO] Command executed: {cmd}")
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Failed running command {cmd}: {e}")

# Load scenario configuration
with open(scenario, 'r') as f:
    data = json.load(f)

scenario = data['scenario']
commands = data['command_to_launch']
command_times = [int(t) for t in data['time']]
time_end = int(data['time_end'])
data_output_name = data['data_output_name']

# Validate ascending order
if any(command_times[i] >= command_times[i + 1] for i in range(len(command_times) - 1)):
    raise ValueError("Time vector must be strictly ascending.")

print(f"[INFO] Starting scenario {scenario}")
subprocess.run(['bash', 'start.sh'], cwd="/home/nos3/Desktop/github-nos3", check=True)
print("[INFO] Mission environment initialized.")

# Start remote tcpdump
ssh_client = start_remote_tcpdump(
    ip_address="192.168.100.5",
    username="nos3",
    password="nos3123!",
    data_output_name=data_output_name
)

# Schedule commands
for t, cmd in zip(command_times, commands):
    run_at(t, lambda cmd=cmd: launch_cmd(cmd))
print("[INFO] Attack scripts scheduled.")

# Schedule end operations
def end_scenario():
    print("[INFO] Ending scenario...")
    stop_remote_tcpdump(ssh_client)
    subprocess.run(['bash', 'stopT.sh'], cwd="/home/nos3/Desktop/github-nos3", check=True)   
    print("[INFO] Simulation cleanup complete.")

run_at(time_end, end_scenario)

print("[INFO] Simulation running. Waiting for completion...")
pytime.sleep(time_end + 10)
print("[INFO] Scenario complete. .pcap available in /tmp on VM MCS.")

