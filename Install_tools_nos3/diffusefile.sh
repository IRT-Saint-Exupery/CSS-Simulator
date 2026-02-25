#!/bin/bash

# has 2 parameter 
# ficdet - destination file 
# IP address of the destination VM

if [ -d $1 ]; then exit; fi

if [[ $1 == /* ]]; then
    export ficdest=$1
else
    export ficdest=`pwd`/$1
fi
  
pathfile=$(dirname $ficdest) 
var="mkdir -p $pathfile"
/usr/bin/sshpass -p nos3123! ssh nos3@$2 $var  
/usr/bin/sshpass -p nos3123! scp $1 nos3@$2:$ficdest
if [ $? -eq 0 ]; then echo $1 copied; fi


