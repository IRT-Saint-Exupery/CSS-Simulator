import paramiko
import sys
import os

# Predefined configuration
remote_machines = {
    '192.168.100.11': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.12': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.13': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.14': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.15': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.16': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
    '192.168.100.17': ['/tmp/logids_p0.csv','/tmp/logids_p1.csv','/tmp/logids_p2.csv','/tmp/logids_p3.csv'],
}

destination_machine = {
    'ip': '192.168.100.5',
    'username': 'nos3',
    'password': 'nos3123!',
}

username = 'nos3'
password = 'nos3123!'


def create_ssh_client(host, username, password):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(hostname=host, username=username, password=password)
    return ssh


def copy_files(destination_path):
    dest_ssh = create_ssh_client(destination_machine['ip'], destination_machine['username'], destination_machine['password'])
    dest_sftp = dest_ssh.open_sftp()

    # Ensure destination directory exists
    try:
        dest_sftp.listdir(destination_path)
    except IOError:
        stdin, stdout, stderr = dest_ssh.exec_command(f"mkdir -p {destination_path}")
        stdout.channel.recv_exit_status()

    for ip, file_list in remote_machines.items():
        print(f"Connecting to {ip}...")
        ssh = create_ssh_client(ip, username, password)
        sftp = ssh.open_sftp()

        for file_path in file_list:
            filename = os.path.basename(file_path)
            parts = filename.rsplit('.', 1)
            if len(parts) == 2:
                unique_filename = f"{parts[0]}_{ip.split('.')[-1]}.{parts[1]}"
            else:
                unique_filename = f"{filename}_{ip.split('.')[-1]}"
            local_temp = f"/tmp/{unique_filename}"

            print(f"Downloading {file_path} from {ip}...")
            sftp.get(file_path, local_temp)

            print(f"Uploading {unique_filename} to {destination_machine['ip']}:{destination_path}...")
            dest_sftp.put(local_temp, os.path.join(destination_path, unique_filename))

        sftp.close()
        ssh.close()

    dest_sftp.close()
    dest_ssh.close()
    print("All files copied successfully.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 copy_files.py <destination_folder>")
        sys.exit(1)

    destination_folder = sys.argv[1]
    copy_files(destination_folder)

