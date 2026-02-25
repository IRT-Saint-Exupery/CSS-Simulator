#!/usr/bin/env python3
import math
from statistics import mean

INPUT_FILE = "/tmp/ctrlErr.txt"

def read_ctrl_err_file(path):
    """Return list of (x, y, z, mse_file) from the input file."""
    data = []
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            # Expect 4 columns: x y z mse
            parts = line.split()
            if len(parts) < 4:
                continue  # or raise an error if you prefer
            x, y, z, mse_file = map(float, parts[:4])
            data.append((x, y, z, mse_file))
    return data

def compute_mse_xyz(x, y, z):
    """MSE = (x^2 + y^2 + z^2) / 3."""
    return (x*x + y*y + z*z) / 3.0

def main():
    data = read_ctrl_err_file(INPUT_FILE)
    if not data:
        print("No valid data found in", INPUT_FILE)
        return

    # Per-sample scalar error from x,y,z: sqrt((x^2+y^2+z^2)/3)
    per_sample_errors = []
    mse_xyz_list = []
    mse_file_list = []

    for x, y, z, mse_file in data:
        mse_xyz = compute_mse_xyz(x, y, z)
        err = math.sqrt(mse_xyz)  # RMS error (scalar) for this sample
        per_sample_errors.append(err)
        mse_xyz_list.append(mse_xyz)
        mse_file_list.append(mse_file)

    # Global pointing accuracy estimate:
    # RMS over all samples (equivalent to sqrt(mean(MSE)))
    mean_mse_xyz = mean(mse_xyz_list)
    rms_error = math.sqrt(mean_mse_xyz)

    print("Number of samples:", len(data))
    print("Mean MSE from x,y,z:", mean_mse_xyz)
    print("Estimated pointing accuracy (RMS angular error):", rms_error)
    print()

    # Optional: compare with MSE given in the file
    mean_mse_file = mean(mse_file_list)
    print("Mean MSE from file (4th column):", mean_mse_file)
    print("RMS from file MSE:", math.sqrt(mean_mse_file))

if __name__ == "__main__":
    main()

