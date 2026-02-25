#!/bin/bash

# Specialize the VM in SAT X

# - the SC ID Source (from 1 to N) - the SC ID of the source SAT  - $1
# - SC ID Destination - the SC ID of the new SAT                  - $2
# - IP Source - IP to be found and in the hierarchy and replaced by the IP Destination (IP of the source SAT) - $3
# - IP Destination - the IP of the VM - $4

if [ $# -ne 4 ]; then
  echo "*********************** Specialize_SAT.sh needs four arguments  ************************"
  exit
fi

#Set NBSAT
source /home/nos3/Install_tools_nos3/IP_Scenario.sh

ID142=$(expr $1 - 1)
ID242=$(expr $2 - 1)

# Set the Spacecraft ID for each satellite 
var="sed -i 's/SET(SPACECRAFT_ID $1)/SET(SPACECRAFT_ID $2)/g' ~/Desktop/github-nos3/fsw/nos3_defs/targets.cmake"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var
var="sed -i 's/g_satId = $1/g_satId = $2/g' ~/Desktop/github-nos3/components/cryptolib/src/core/crypto_config.c"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Set spacecraft number for 42 (should be coherent with SC ID)
var="sed -i 's/SC\\[$ID142\\]/SC\\[$ID242\\]/g' ~/Desktop/github-nos3/components/generic_reaction_wheel/sim/src/generic_rw_hardware_model.cpp"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var
var="sed -i 's/craft>$ID142<\\/space/craft>$ID242<\\/space/g' ~/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var


# Set the correct IP address for the SAT X
listIPAddrFiles=("sims/sim_common/src/sim_data_42socket_provider.cpp" "sims/truth_42_sim/src/truth_42_hardware_model.cpp" "sims/cfg/nos3-simulator.xml" "sims/cfg/nos3-simulator-init.xml" "sims/cfg/nos_engine_server_config.json" "sims/cfg/InOut/Inp_NOS3.txt" "sims/sim_terminal/src/simulator_terminal.cpp" "sims/sim_terminal/cfg/nos3-terminal-simulator.xml" "sims/nos_time_driver/src/time_driver.cpp" "components/generic_mag/sim/src/generic_mag_hardware_model.cpp" "components/generic_torquer/sim/src/generic_torquer_hardware_model.cpp" "components/generic_imu/sim/src/generic_imu_hardware_model.cpp" "components/generic_css/sim/src/generic_css_hardware_model.cpp" "components/sample/sim/src/sample_hardware_model.cpp" "components/isl/sim/src/isl_hardware_model.cpp" "components/arducam/sim/src/cam_hardware_model.cpp" "components/novatel_oem615/sim/src/gps_sim_hardware_model_OEM615.cpp" "components/novatel_oem615/sim/src/gps_sim_data_file_provider.cpp" "components/generic_radio/sim/src/generic_radio_hardware_model.cpp" "components/generic_eps/sim/src/generic_eps_hardware_model.cpp" "components/generic_reaction_wheel/sim/src/generic_rw_hardware_model.cpp" "components/generic_fss/sim/src/generic_fss_hardware_model.cpp" "components/generic_star_tracker/sim/src/generic_star_tracker_hardware_model.cpp" "fsw/apps/hwlib/sim/src/nos_link.c" "fsw/psp/fsw/nos-linux/src/cfe_psp_start.c" "fsw/nos3_defs/tables/sc_rts001.c" "gsw/scripts/python-cfdp-main/examples/spacepacket_transport/remote.py" "gsw/scripts/python-cfdp-main/examples/spacepacket_transport/remoteGND.py")

echo $4

for i in ${!listIPAddrFiles[@]}; 
do 
var="sed -i 's/$3/$4/g' ~/Desktop/github-nos3/${listIPAddrFiles[$i]}"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var
echo ${listIPAddrFiles[$i]}
done

# Particularise the FSS port for 42
portBaseFss42=4281
newPortFss42=$(expr $portBaseFss42 + 1000 \* $ID242)

var="sed -i 's/$portBaseFss42/$newPortFss42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the ST0 port for 42
portBaseST42=4282
newPortST42=$(expr $portBaseST42 + 1000 \* $ID242)

var="sed -i 's/$portBaseST42/$newPortST42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the ST1 port for 42
portBaseST42=4283
newPortST42=$(expr $portBaseST42 + 1000 \* $ID242)

var="sed -i 's/$portBaseST42/$newPortST42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the RW0 ports for 42
portBaseRW42=4278
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

portBaseRW42=4277
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the RW1 ports for 42
portBaseRW42=4378
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

portBaseRW42=4377
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the RW2 ports for 42
portBaseRW42=4478
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

portBaseRW42=4477
newPortRW42=$(expr $portBaseRW42 + 1000 \* $ID242)

var="sed -i 's/$portBaseRW42/$newPortRW42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var

# Particularise the GPS ports for 42
portBaseGPS42=4245
newPortGPS42=$(expr $portBaseGPS42 + 1000 \* $ID242)

var="sed -i 's/$portBaseGPS42/$newPortGPS42/g' /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml"
/usr/bin/sshpass -p nos3123! ssh nos3@$4 $var


# specialize makefile and launch
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/MakefileSAT ~/Desktop/github-nos3/Makefile" &

# update 42 files
nom=Inp_Cmd.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Cmd.txt" &

nom=Inp_IPC.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_IPC.txt" &

nom=Inp_Sim.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt" &
# init to modify the time
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim_init.txt" &

nom=Inp_IPC.txt.TX.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; sudo cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /opt/nos3/42/Tx/Inp_IPC.txt" &

nom=Inp_IPC.txt.RX.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; sudo cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /opt/nos3/42/Rx/Inp_IPC.txt" &

nom=routeConfig.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/RouteConfig/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/routeConfig.txt" &

#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$4 "export DISPLAY=:0; sudo cp -r /home/nos3/Desktop/github-nos3/sims/cfg/InOut/* /opt/nos3/42/InOut/" &



