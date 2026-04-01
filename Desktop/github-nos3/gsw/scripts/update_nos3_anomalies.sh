#!/usr/bin/env bash
# update_nos3_anomalies.sh <SAT_ID:TRUE_ANOMALY_DEG> [<SAT_ID:TRUE_ANOMALY_DEG> ...]
#
# Updates the True Anomaly field in the local Orb_ISS_sat<N>.txt config files
# for the specified satellites on the current machine.
#
# Each argument must be formatted as "sat_id:true_anomaly_deg", e.g.:
#   update_nos3_anomalies.sh 1:257.145 2:3.789
#
# This script is typically not called directly. It is invoked by
# diffuse_nos3_anomalies.sh on every VM in the network (42, MCS, SAT1, SAT2…).
#
# The caller is responsible for computing the correct anomaly
# values for the chosen epoch. This script is a pure setter.

set -euo pipefail

ORB_DIR="/home/nos3/Desktop/github-nos3/sims/cfg/InOut"

if [ $# -eq 0 ]; then
    echo "Usage: $0 SAT_ID:TRUE_ANOMALY_DEG [SAT_ID:TRUE_ANOMALY_DEG ...]" >&2
    echo "Example: $0 1:257.145 2:3.789 3:110.0" >&2
    exit 1
fi

for pair in "$@"; do
    # Split on first ':' to allow negative or decimal angles
    sat_id="${pair%%:*}"
    angle="${pair#*:}"

    if [ -z "$sat_id" ] || [ -z "$angle" ]; then
        echo "Error: malformed argument '${pair}', expected SAT_ID:ANGLE_DEG" >&2
        exit 1
    fi

    file="${ORB_DIR}/Orb_ISS_sat${sat_id}.txt"

    if [ ! -f "$file" ]; then
        echo "Error: orbit file not found: $file" >&2
        exit 1
    fi

    # Normalise angle to [0, 360) and overwrite the True Anomaly line in-place
    normalised=$(awk -v a="$angle" 'BEGIN { a = ((a % 360) + 360) % 360; printf "%.6f", a }')
    tmp_file="$(mktemp)"
    awk -v angle="$normalised" '{
        if ($0 ~ /True Anomaly \(deg\)/) {
            printf "%.3f                         !  True Anomaly (deg)\n", angle
        } else {
            print
        }
    }' "$file" > "$tmp_file"
    mv "$tmp_file" "$file"

    echo "Satellite ${sat_id}: True Anomaly set to ${normalised} deg in $(basename "$file")"
done
