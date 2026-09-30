#!/bin/bash

# config IP address for your scenario 

# Preliminary definition of IP configuration (to be adapted according to your configuration)

IP_LOCAL_ADDR=$(hostname -I | awk '{print $1}')

NETWORK_BASE_IP=$(echo $IP_LOCAL_ADDR | cut -d "." -f 1-3)
#echo $NETWORK_BASE_IP

NBSAT=1  ### <----------- Modify here : It should correspond to your scenario ! (same for the IP)
# IPSAT1="192.168.100.11"
# IPSAT2="192.168.100.12"
# IPSAT3="192.168.100.13"
# IPSAT4="192.168.100.14"
# IPSAT5="192.168.100.15"
# IPSAT6="192.168.100.16"
# IPSAT7="192.168.100.17"
# IP42="192.168.100.42"
# IPMCS="192.168.100.5"
IPSAT1="$NETWORK_BASE_IP.11"
IPSAT2="$NETWORK_BASE_IP.12"
IPSAT3="$NETWORK_BASE_IP.13"
IPSAT4="$NETWORK_BASE_IP.14"
IPSAT5="$NETWORK_BASE_IP.15"
IPSAT6="$NETWORK_BASE_IP.16"
IPSAT7="$NETWORK_BASE_IP.17"
IP42="$NETWORK_BASE_IP.42"
IPMCS="$NETWORK_BASE_IP.5"