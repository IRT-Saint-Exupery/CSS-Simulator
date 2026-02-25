#!/bin/bash

# script to resend traffic to GW probes

# IP of this machine
IP_CURRENT=$(hostname -I | awk '{print $1}')

# send traffic from MCS to GW VM
INTERFACE="eth0"

DEST_IP="192.168.100.8"

DEST_PORT="1234"

# this is on VM .5 
if [ "$IP_CURRENT" = "192.168.100.5" ]; then 
# traffic nord-sud (space link)
sudo tcpdump -i $INTERFACE -w - | nc $DEST_IP $DEST_PORT
# traffic all
# sudo tcpdump -i any -w - | nc $DEST_IP $DEST_PORT
fi

# send traffic from GW VM to GCAP
LISTEN_PORT="1234"

# GCAP hybrid network
NEW_IP="192.168.111.2"

# GCAP Port
NEW_PORT="12345"

# this is on GW VM (resend traffic to the probe)
if [ "$IP_CURRENT" = "192.168.100.8" ]; then 
sudo nc -l -p $LISTEN_PORT | nc $NEW_IP $NEW_PORT  
fi

