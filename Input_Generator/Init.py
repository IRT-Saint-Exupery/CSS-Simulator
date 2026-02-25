import sys
import subprocess

def main():
    ports = [6010, 6012, 6014, 6016, 6018, 6020, 6022] #Cryptolib Standalone ports for Input from 1 to 7 Satellites.

    if len(sys.argv) != 2:
        print("Usage: python Init.py <N from 1 to 7>")
        sys.exit(1)

    try:
        N = int(sys.argv[1])
    except ValueError:
        print("Argument must be an integer between 1 and 7")
        sys.exit(1)

    if N < 1 or N > 7:
        print("Argument must be between 1 and 7")
        sys.exit(1)

    for i in range(N):
        port = ports[i]
        print(f"Launching Init_mission.py with port {port} for SAT {i+1}")
        subprocess.run(["python3", "Init_mission.py", str(port)],cwd="/home/nos3/Input_Generator/")

if __name__ == "__main__":
    main()

