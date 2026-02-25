import subprocess
import time
import sys

if len(sys.argv) != 2:
    print(f"Usage: python {sys.argv[0]} <port>")
    sys.exit(1)

port = sys.argv[1]

subprocess.run(["python3", "Main.py", "send", "scenarios/novatel_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/st_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/css_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/fss_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/torquer_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/mag_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/imu_enable.json", "--port", port])
time.sleep(4)
subprocess.run(["python3", "Main.py", "send", "scenarios/adcs_MM_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/adcs_Mode.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/ids_enable.json", "--port", port])
time.sleep(1)
subprocess.run(["python3", "Main.py", "send", "scenarios/to_enable_output.json", "--port", port])
time.sleep(1)
