import subprocess
from pathlib import Path
import time
import random
import os

# Point to your existing Bash script
SCRIPT_PATH = Path("/home/nos3/CSS_Attacks/Restore_CAM.sh")


def wait_before_run():
    # Wait random time between 10 and 40 seconds
    delay = random.randint(10, 40)
    print(f"Initial random delay: {delay} seconds")
    time.sleep(delay)

    # Check file for up to 5 minutes
    timeout = 300  # 5 minutes
    check_interval = 5
    start_time = time.time()

    while time.time() - start_time < timeout:
        try:
            with open("/tmp/isbusy.txt", "r") as f:
                content = f.read().strip()

            # Get first number in file
            first_value = content.split()[0]

            if first_value == "0":
                print("SAT not busy. Running Restore_CAM")
                return True
            else:
                print("SAT busy. Waiting 5 seconds...")
        except (FileNotFoundError, IndexError):
            print("File missing or empty. Waiting 5 seconds...")

        time.sleep(check_interval)

    print("Timeout reached (5 minutes). Not running Restore_CAM.")
    return False


def run_bash():
    subprocess.run(
        [str(SCRIPT_PATH)],
        cwd="/home/nos3/CSS_Attacks",
        shell=True,
        executable="/bin/bash",
        check=True
    )

if __name__ == "__main__":
    if wait_before_run():
        run_bash()   


