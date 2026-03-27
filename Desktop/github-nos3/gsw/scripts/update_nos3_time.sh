#!/usr/bin/env bash
# Usage: ./update_nos3_time.sh <delta_seconds>

if [ $# -ne 1 ]; then
    echo "Usage: $0 <delta_seconds>"
    exit 1
fi

DELTA=$1

XML_INIT="/home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator-init.xml"
XML_OUT="/home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
INP_INIT="/home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim_init.txt"
INP_OUT="/home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt"

# 1) XML 
ORIG_ABS=$(grep -oP '(?<=<absolute-start-time>)[0-9.]+(?=</absolute-start-time>)' "$XML_INIT")
ORIG_INT=${ORIG_ABS%.*}
FRAC=${ORIG_ABS#"$ORIG_INT"}
NEW_INT=$((ORIG_INT + DELTA))
NEW_ABS="${NEW_INT}${FRAC}"

sed "s|<absolute-start-time>${ORIG_ABS}</absolute-start-time>|<absolute-start-time>${NEW_ABS}</absolute-start-time>|" "$XML_INIT" > "$XML_OUT"

# 2) TXT - use epoch seconds, write to temp files first
BASE_EPOCH=$(date -u -d '2025-10-18 08:30:00' +%s)
NEW_EPOCH=$((BASE_EPOCH + DELTA))

# Create new lines in temp files (safe quoting)
date -u -d "@$NEW_EPOCH" '+%m %d %Y                      !  Date (UTC) (Month, Day, Year)' > /tmp/new_date_line
date -u -d "@$NEW_EPOCH" '+%H %M %S.%02N                     !  Time (UTC) (Hr,Min,Sec)' > /tmp/new_time_line

# Replace lines using temp files (proper sed syntax with variables)
sed "/Date (UTC)/c $(cat /tmp/new_date_line)" "$INP_INIT" | \
sed "/Time (UTC)/c $(cat /tmp/new_time_line)" > "$INP_OUT"

rm -f /tmp/new_date_line /tmp/new_time_line

