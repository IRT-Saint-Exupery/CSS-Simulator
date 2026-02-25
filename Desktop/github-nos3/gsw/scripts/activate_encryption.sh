#!/bin/bash -i
#
#

#Set NBSAT
source /home/nos3/Install_tools_nos3/IP_Scenario.sh

./send_vcid.sh 1 4
if [ $NBSAT -ge 2 ]; then ./send_vcid.sh 2 4; fi
if [ $NBSAT -ge 3 ]; then ./send_vcid.sh 3 4; fi
if [ $NBSAT -ge 4 ]; then ./send_vcid.sh 4 4; fi
if [ $NBSAT -ge 5 ]; then ./send_vcid.sh 5 4; fi
if [ $NBSAT -ge 6 ]; then ./send_vcid.sh 6 4; fi
if [ $NBSAT -ge 7 ]; then ./send_vcid.sh 7 4; fi
