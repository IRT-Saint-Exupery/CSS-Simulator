#!/usr/bin/env bash
# update true anomaly (ok only for constant rate with e=0 !!!)

set -euo pipefail

if [ $# -ne 3 ]; then
  echo "Usage: $0 INIT_ANGLE_DEG DELTA_SEC FILE"
  exit 1
fi

init_angle="$1"
delta_sec="$2"
file="$3"

coeff="0.0594804"  # deg per sec (constant rate hypothesis !!!)

tmp_file="$(mktemp)"

awk -v init="$init_angle" -v delta="$delta_sec" -v coeff="$coeff" '
BEGIN {
  new_angle = init + delta * coeff
  new_angle = ((new_angle % 360) + 360) % 360
}
{
  if ($0 ~ /True Anomaly \(deg\)/) {
    printf "%.3f                         !  True Anomaly (deg)\n", new_angle
  } else {
    print
  }
}
' "$file" > "$tmp_file"

mv "$tmp_file" "$file"
