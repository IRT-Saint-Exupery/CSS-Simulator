#/bin/bash

# SAT=$1
# vcid=$2  #1 for clear and 4 for encrypted 

num1=$(ps -elf | grep "standalone $1" | awk '{print $4}' | head -n1)

#echo $num1

fd="/proc/${num1}/fd/0"

#echo $fd

sudo ./external_command_ingest.out -n $fd "vcid $2"

