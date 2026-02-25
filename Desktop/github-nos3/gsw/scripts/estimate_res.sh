#!/bin/bash

HEADROOM_CPU=1.3     # 30% extra CPU
HEADROOM_RAM=1.3     # 30% extra RAM
MIN_RAM_GB=2

TOP_DATA=$(top -bn1)

# ---- CPU usage (user + system) ----
CPU_USAGE=$(echo "$TOP_DATA" \
  | awk -F'[, ]+' '/^%Cpu\(s\):/ {
        for (i = 1; i <= NF; i++) {
            if ($i ~ /us$/) us=$(i-1)
            if ($i ~ /sy$/) sy=$(i-1)
        }
        printf "%d\n", us + sy
    }')
CPU_USAGE=${CPU_USAGE:-5}

TOTAL_CORES=$(grep -c '^processor' /proc/cpuinfo 2>/dev/null || echo 2)

# ---- Memory usage (MiB Mem) ----
MEM_LINE=$(echo "$TOP_DATA" | awk '/MiB Mem/ {print; exit}')

# Extract numeric fields ignoring punctuation
TOTAL_RAM_MiB=$(echo "$MEM_LINE" | awk '{for(i=1;i<=NF;i++) if($i=="total,") {print $(i-1); exit}}')
USED_RAM_MiB=$(echo "$MEM_LINE"  | awk '{for(i=1;i<=NF;i++) if($i=="used,")  {print $(i-1); exit}}')

TOTAL_RAM_MiB=${TOTAL_RAM_MiB%%.*}
USED_RAM_MiB=${USED_RAM_MiB%%.*}

RAM_PCT=0
if [ "$TOTAL_RAM_MiB" -gt 0 ]; then
  RAM_PCT=$(( USED_RAM_MiB * 100 / TOTAL_RAM_MiB ))
fi

TOTAL_RAM_GB=$(echo "scale=1; $TOTAL_RAM_MiB / 1024" | bc)

# Round used MiB to GiB
ACTUAL_USED_GB_INT=$(( (USED_RAM_MiB + 1023) / 1024 ))

# ---- Recommended sizing ----
# CPU: scale cores according to average util with headroom
RAW_CORES=$(echo "$CPU_USAGE $TOTAL_CORES $HEADROOM_CPU" \
  | awk '{printf "%d\n", int( ($1/100.0)*$2*$3 + 0.5 )}')
[ "$RAW_CORES" -lt 1 ] && RAW_CORES=1

# RAM: used * headroom, ceiling, min
EST_RAM_GB=$(echo "$ACTUAL_USED_GB_INT $HEADROOM_RAM" \
  | awk '{printf "%d\n", int($1*$2 + 0.5)}')
[ "$EST_RAM_GB" -lt "$MIN_RAM_GB" ] && EST_RAM_GB=$MIN_RAM_GB

echo "Current system snapshot:"
echo "CPU Usage (user+sys): ${CPU_USAGE}% across ${TOTAL_CORES} cores"
echo "RAM Usage: ${RAM_PCT}% (${TOTAL_RAM_GB} GB total)"
echo "Actual used: ${USED_RAM_MiB} MiB (~${ACTUAL_USED_GB_INT} GB)"

echo ""
echo "## Recommended VM allocation (~30% headroom):"
echo "- CPU cores: ${RAW_CORES}"
echo "- RAM: ${EST_RAM_GB} GB (for ~${ACTUAL_USED_GB_INT} GB used)"
echo ""
echo "Note: Run this 3+ times during peak load and average the results."

