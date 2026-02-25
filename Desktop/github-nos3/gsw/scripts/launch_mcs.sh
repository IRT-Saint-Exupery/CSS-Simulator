#!/bin/bash -i
#
# Convenience script for NOS3 development
#

#Set NBSAT
source /home/nos3/Install_tools_nos3/IP_Scenario.sh
echo "There are $NBSAT Satellites in the Constellation"


SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
#SIMS=$(cd $SIM_BIN; ls nos3*simulator)

sudo chmod 775 -R $SIM_BIN
sudo chmod 775 -R $BASE_DIR/components/cryptolib/build/

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "FSW directory    = " $FSW_BIN
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#exit

#echo "Make /tmp folders..."
mkdir /tmp/data 2> /dev/null
mkdir /tmp/data/hk 2> /dev/null
mkdir /tmp/uplink 2> /dev/null

echo "Make data folders..."
# FSW Side
mkdir $FSW_BIN/data 2> /dev/null
mkdir $FSW_BIN/data/cam 2> /dev/null
mkdir $FSW_BIN/data/evs 2> /dev/null
mkdir $FSW_BIN/data/hk 2> /dev/null
mkdir $FSW_BIN/data/inst 2> /dev/null
# GSW Side
mkdir /tmp/data 2> /dev/null
mkdir /tmp/data/cam 2> /dev/null
mkdir /tmp/data/evs 2> /dev/null
mkdir /tmp/data/hk 2> /dev/null
mkdir /tmp/data/inst 2> /dev/null
mkdir /tmp/uplink 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/uplink/tmp0.so 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/uplink/tmp1.so 2> /dev/null

mkdir -p $BASE_DIR/fsw/build/exe/cpu1/cf
cp $BASE_DIR/sims/cfg/routeConfig.txt $BASE_DIR/fsw/build/exe/cpu1/cf/routeConfig.txt


echo "CryptoLib..."
mkdir $BASE_DIR/components/cryptolib/build/
cd $BASE_DIR/components/cryptolib/build/
export CFLAGS="-m32"
cmake .. -DSUPPORT=1 && make -j2
## gnome-terminal --tab --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone
gnome-terminal --tab --title="CryptoLib SAT1" -- $BASE_DIR/components/cryptolib/build/support/standalone 1 
if [ $NBSAT -ge 2 ]; then gnome-terminal --tab --title="CryptoLib SAT2" -- $BASE_DIR/components/cryptolib/build/support/standalone 2; fi
if [ $NBSAT -ge 3 ]; then gnome-terminal --tab --title="CryptoLib SAT3" -- $BASE_DIR/components/cryptolib/build/support/standalone 3; fi
if [ $NBSAT -ge 4 ]; then gnome-terminal --tab --title="CryptoLib SAT4" -- $BASE_DIR/components/cryptolib/build/support/standalone 4; fi
if [ $NBSAT -ge 5 ]; then gnome-terminal --tab --title="CryptoLib SAT5" -- $BASE_DIR/components/cryptolib/build/support/standalone 5; fi
if [ $NBSAT -ge 6 ]; then gnome-terminal --tab --title="CryptoLib SAT6" -- $BASE_DIR/components/cryptolib/build/support/standalone 6; fi
if [ $NBSAT -ge 7 ]; then gnome-terminal --tab --title="CryptoLib SAT7" -- $BASE_DIR/components/cryptolib/build/support/standalone 7; fi

## Note: Can keep open if desired after a new gnome-profile is manually created
##cmake .. -DDEBUG=1 && make -j2
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 1
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 2
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 3
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 4
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 5
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 6
#gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone 7


sleep 10

## CFDP tabs
cd /home/nos3/Desktop/github-nos3/gsw/scripts/python-cfdp-main
gnome-terminal --tab --title='CFDP server SAT1'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5013; exec bash -i"
if [ $NBSAT -ge 2 ]; then gnome-terminal --tab --title='CFDP server SAT2'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5113; exec bash -i"; fi
if [ $NBSAT -ge 3 ]; then gnome-terminal --tab --title='CFDP server SAT3'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5213; exec bash -i"; fi
if [ $NBSAT -ge 4 ]; then gnome-terminal --tab --title='CFDP server SAT4'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5313; exec bash -i"; fi
if [ $NBSAT -ge 5 ]; then gnome-terminal --tab --title='CFDP server SAT5'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5413; exec bash -i"; fi
if [ $NBSAT -ge 6 ]; then gnome-terminal --tab --title='CFDP server SAT6'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5513; exec bash -i"; fi
if [ $NBSAT -ge 7 ]; then gnome-terminal --tab --title='CFDP server SAT7'             -- bash -c  "python3.8 examples/spacepacket_transport/remoteGND.py 5613; exec bash -i"; fi

sleep 1

echo "COSMOS Ground Station..."
#cd $BASE_DIR/gsw/cosmos
#export MISSION_NAME=$(echo "NOS3")
#export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
#ruby Launcher -c nos3_launcher.txt --system nos3_system.txt &
pidof firefox > /dev/null
if [ $? -eq 1 ]
then
if [ $NBSAT -eq 1 ]; then firefox --new-tab localhost:2900 & fi
if [ $NBSAT -eq 2 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 & fi # 2 SAT
if [ $NBSAT -eq 3 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 --new-tab localhost:2902 & fi # 3 SAT
if [ $NBSAT -eq 4 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 --new-tab localhost:2902 --new-tab localhost:2903 & fi # 4 SAT
if [ $NBSAT -eq 5 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 --new-tab localhost:2902 --new-tab localhost:2903 --new-tab localhost:2904 & fi # 5 SAT
if [ $NBSAT -eq 6 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 --new-tab localhost:2902 --new-tab localhost:2903 --new-tab localhost:2904 --new-tab localhost:2905 & fi # 6 SAT
if [ $NBSAT -eq 7 ]; then firefox --new-tab localhost:2900 --new-tab localhost:2901 --new-tab localhost:2902 --new-tab localhost:2903 --new-tab localhost:2904 --new-tab localhost:2905 -new-tab localhost:2906 & fi # 7 SAT

# show the mission map
sleep 1
python3.8 /home/nos3/eclipse-workspace/mission/src/add_targets.py
sleep 10
cp /home/nos3/eclipse-workspace/mission/src/Mission_start_target.png /home/nos3/eclipse-workspace/mission/src/Mission.png
firefox --new-tab "file:////home/nos3/eclipse-workspace/mission/src/Mission.png"
fi

sleep 1
# launch frontend 
DIR="/home/nos3/eclipse-workspace/frontend"
gnome-terminal --tab --title="Front-End" -- $DIR/Release/frontend $DIR/config/netConfig.txt $DIR/config/routeConfig.txt $DIR/config/flagsConfig.txt 
#gnome-terminal --window-with-profile=KeepOpen --title="Front-End" -- $DIR/Release/frontend $DIR/config/netConfig.txt $DIR/config/routeConfig.txt $DIR/config/flagsConfig.txt


sleep 1
# launch mission 
DIR="/home/nos3/eclipse-workspace/mission"
gnome-terminal --tab --title="Mission" -- $DIR/Release/mission  
#gnome-terminal --window-with-profile=KeepOpen --title="Mission" -- $DIR/Release/mission



#echo "Flight Software..."
#cd $FSW_BIN
#$SCRIPT_DIR/fsw_respawn.sh $FSW_BIN &
# Note: Can keep open if desired after a new gnome-profile is manually created
#gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &
