#/bin/sh

sshpass -p nos3123! ssh nos3@192.168.100.8 "cd Desktop/github-nos3; make launch"
sshpass -p nos3123! ssh nos3@192.168.100.42 "cd Desktop/github-nos3; make launch"
make launch
