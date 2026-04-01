#!/bin/bash
# diffuse_nos3_anomalies.sh <SAT_ID:TRUE_ANOMALY_DEG> [<SAT_ID:TRUE_ANOMALY_DEG> ...]
#
# Updates the True Anomaly fields in Orb_ISS_sat<N>.txt on ALL VMs in the
# network (42, MCS, SAT1, and any additional satellite VMs) by calling
# update_nos3_anomalies.sh on each of them.
#
# Each argument must be formatted as "sat_id:true_anomaly_deg", e.g.:
#   diffuse_nos3_anomalies.sh 1:257.145 2:3.789
#
# Should be called from SAT1 only.

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
    echo "#################### $0 Should be called from the main VM - SAT1 #################"
    exit
fi

if [ $# -eq 0 ]; then
    echo "Usage: $0 SAT_ID:TRUE_ANOMALY_DEG [SAT_ID:TRUE_ANOMALY_DEG ...]" >&2
    echo "Example: $0 1:257.145 2:3.789 3:110.0" >&2
    exit 1
fi

UPDATE_SCRIPT="/home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_anomalies.sh"

# call for 42
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "export DISPLAY=:0; . $UPDATE_SCRIPT $*"

# call for MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:0; . $UPDATE_SCRIPT $*"

# call for SAT1 (local)
. $UPDATE_SCRIPT "$@"

# call for other SATs
if [ $NBSAT -ge 2 ]; then
    if ping -c 1 "$IPSAT2" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT2 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT2 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

if [ $NBSAT -ge 3 ]; then
    if ping -c 1 "$IPSAT3" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT3 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT3 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

if [ $NBSAT -ge 4 ]; then
    if ping -c 1 "$IPSAT4" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT4 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT4 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

if [ $NBSAT -ge 5 ]; then
    if ping -c 1 "$IPSAT5" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT5 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT5 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

if [ $NBSAT -ge 6 ]; then
    if ping -c 1 "$IPSAT6" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT6 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT6 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

if [ $NBSAT -ge 7 ]; then
    if ping -c 1 "$IPSAT7" &>/dev/null; then
        echo "########## UPDATE ANOMALY SAT7 #########"
        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT7 "export DISPLAY=:0; . $UPDATE_SCRIPT $*" &
    fi
fi

wait
