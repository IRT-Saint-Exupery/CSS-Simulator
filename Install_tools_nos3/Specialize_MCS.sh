#!/bin/bash

# specialize makefile and launch
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/MakefileMCS /home/nos3/Desktop/github-nos3/Makefile" &

