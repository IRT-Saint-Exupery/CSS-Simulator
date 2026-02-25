#!/bin/bash

#wait before sending attacks, don't send them right away

sleep 1h

#random sleep of 1-60mn
MOD=60

nb4=$RANDOM

let "nb4%=$MOD"
let "nb4+=1"

sleep "${nb4}m"

echo "DEBUG : ${nb4}m elapsed"

echo "swap cam file to flooding payload"
#swap cam file to flooding payload

/bin/bash Attack_A13_logging.sh

###sleep 10m

#random sleep of 5-65mn
nb=$RANDOM

let "nb%=$MOD"
let "nb+=5"

sleep "${nb}m"

echo "DEBUG : ${nb}m elapsed"

python3 ../Input_Generator/Main.py send ../Input_Generator/cam_hw_check.json

###sleep 15m

#random sleep of 1-60mn
nb2=$RANDOM

let "nb2%=$MOD"
let "nb2+=1"

sleep "${nb2}m"

echo "DEBUG : ${nb2}m elapsed"

#echo "swap cam file to delete /cf payload"
echo "swap cam file to flood payload with hk"

#swap cam file to delete /cf payload

######/bin/bash Attack_A14_delete.sh
/bin/bash Attack_A13_v2.sh

###sleep 20m

#random sleep of 5-65mn
nb3=$RANDOM

let "nb3%=$MOD"
let "nb3+=5"

sleep "${nb3}m"

echo "DEBUG : ${nb3}m elapsed"

python3 ../Input_Generator/Main.py send ../Input_Generator/cam_exp3_cc.json

echo "Attacks executed"
