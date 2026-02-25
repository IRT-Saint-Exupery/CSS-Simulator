#!/bin/bash

# 1 - IP of the 42 VM

#diffuse modifactions on 42 
./diffuse.sh $1

./Specialize_42.sh $1

touch ~/cookies/maj$1
