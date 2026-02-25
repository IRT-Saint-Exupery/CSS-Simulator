#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)

#echo "chmod 775 on build/bin"
sudo chmod 775 -R $SIM_BIN

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "FSW directory    = " $FSW_BIN
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "Clean some files"
rm /tmp/coord.txt
rm /tmp/ctrlErr.txt

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

cp $BASE_DIR/sims/cfg/routeConfig.txt $BASE_DIR/fsw/build/exe/cpu1/cf/routeConfig.txt

#echo "42..."
#cd /opt/nos3/42/
#rm -rf NOS3InOut
#cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut
#gnome-terminal --tab --title="42 Dynamic Simulator" -- /opt/nos3/42/42 NOS3InOut

#cd /home/nos3/Desktop/github-nos3
#gnome-terminal --tab --title='github-nos3'

echo "Simulators..."
cd $SIM_BIN
gnome-terminal --tab --title="NOS Engine Server"  -- /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver"    -- $SIM_BIN/nos3-single-simulator time
gnome-terminal --tab --title="NOS STDIO Terminal" -- $SIM_BIN/nos3-single-simulator stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"   -- $SIM_BIN/nos3-single-simulator udp-terminal
gnome-terminal --tab --title="42 Truth Sim"       -- $SIM_BIN/nos3-single-simulator truth42sim
gnome-terminal --tab --title='CAM Sim'            -- $SIM_BIN/nos3-single-simulator camsim
gnome-terminal --tab --title='CSS Sim'            -- $SIM_BIN/nos3-single-simulator generic_css_sim
gnome-terminal --tab --title='EPS Sim'            -- $SIM_BIN/nos3-single-simulator generic_eps_sim
gnome-terminal --tab --title="FSS Sim"            -- $SIM_BIN/nos3-single-simulator generic-fss-sim
gnome-terminal --tab --title='IMU Sim'            -- $SIM_BIN/nos3-single-simulator generic_imu_sim
gnome-terminal --tab --title='MAG Sim'            -- $SIM_BIN/nos3-single-simulator generic_mag_sim
#gnome-terminal --tab --title='Radio Sim'          -- $SIM_BIN/nos3-single-simulator generic_radio_sim
sleep 1
gnome-terminal --tab --title='RW 0 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim0
gnome-terminal --tab --title='RW 1 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim1
gnome-terminal --tab --title='RW 2 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim2
gnome-terminal --tab --title='Torquer Sim'        -- $SIM_BIN/nos3-single-simulator generic_torquer_sim
gnome-terminal --tab --title='GPS Sim'            -- $SIM_BIN/nos3-single-simulator gps
gnome-terminal --tab --title='StarTracker 0 Sim'    -- $SIM_BIN/nos3-single-simulator generic_star_tracker_sim-sim0
gnome-terminal --tab --title='StarTracker 1 Sim'    -- $SIM_BIN/nos3-single-simulator generic_star_tracker_sim-sim1
#gnome-terminal --tab --title='IDS Sim'            -- $SIM_BIN/nos3-single-simulator ids_sim
#gnome-terminal --tab --title='Sample Sim'         -- $SIM_BIN/nos3-single-simulator sample_sim
#gnome-terminal --tab --title='Isl Sim'             -- $SIM_BIN/nos3-single-simulator isl_sim
# for debug of isl
#gnome-terminal --window-with-profile=KeepOpen --title='Isl Sim'             -- $SIM_BIN/nos3-single-simulator isl_sim

cd /home/nos3/Desktop/github-nos3/gsw/scripts/python-cfdp-main
gnome-terminal --tab --title='CFDP server'             -- bash -c  "python3.8 examples/spacepacket_transport/remote.py; exec bash -i"

sleep 10

echo "Flight Software..."
cd $FSW_BIN
$SCRIPT_DIR/fsw_respawn.sh $FSW_BIN &

cd /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1
sudo chmod +x core-cpu1
gnome-terminal --tab --title="NOS3 Flight Software" -- ./core-cpu1 -R PO &
#gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- ./core-cpu1 -R PO &
#sudo -H gnome-terminal --tab --title="NOS3 Flight Software" -- ./core-cpu1 -R PO 

#sudo -H gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- ./core-cpu1 -R PO 
# Note: Can keep open if desired after a new gnome-profile is manually created
#gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &

sleep 60
cd /home/nos3/Desktop/github-nos3/
./gsw/scripts/launch_imager.sh

