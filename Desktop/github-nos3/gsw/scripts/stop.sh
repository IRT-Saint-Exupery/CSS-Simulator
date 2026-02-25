#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)

# cFS
killall -q -r -9 fsw_respawn.sh
killall -q -r -9 core-cpu*

# COSMOS
#killall -q -9 ruby
#yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
#yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
#yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null
#killall -q -9 firefox

# CryptoLib
killall -q -r -9 standalone

# NOS3
killall -q -r -9 'nos3.*simulator.*'
killall -q -r -9 nos3-*
killall -q nos_engine_server_standalone
killall -q nos-time-driver

# NOS3 GPIO
rm -rf /tmp/gpio_fake

# NOS3 Stored HK
rm -rf $BASE_DIR/fsw/build/exe/cpu1/scratch/*

# 42
killall -q 42
#rm -rf /opt/nos3/42/NOS3InOut
rm -rf /tmp/gpio*

pkill imager 
rm /home/nos3/eclipse-workspace/imager/core.imager*

pkill firefox

#pkill -9 python3

# Get PID of manager.py to exclude
MANAGER_PID=$(pgrep -f manager.py)
TERMINATOR_PID=$(pgrep -f terminator)

# Get all python3 PIDs and exclude the manager PID if it exists
python3_pids=$(pgrep -f python3.8 | grep -v "^${MANAGER_PID}$" | grep -v "^${TERMINATOR_PID}$")

# Kill the remaining python3 processes with SIGKILL (-9)
echo "$python3_pids" | xargs -r pkill -9 -P
#echo "$python3_pids" 

pkill -9 -f /home/nos3/eclipse-workspace/mission/Release/mission

pkill gnome-terminal

pkill -f "examples/spacepacket_transport/remote"
pkill -f "/gsw/scripts/launch_sat.sh" # in case sim needs to be stopped quickly after starting

exit 0
