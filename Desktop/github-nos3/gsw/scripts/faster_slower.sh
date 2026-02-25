#/bin/bash

# f_or_d=$1  #f for faster , d for slower 
# example: faster_slower.sh f to accelerate x2 or faster_slower.sh d to decelerate x2

PID=$(ps aux | grep "nos3-single-simulator time" | awk '{print $2}' | head -n1)

fd="/proc/${PID}/fd/0"

sudo ./external_command_ingest.out -n $fd "$1"

