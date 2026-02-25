#!/bin/bash

# 1 - IP of the MCS VM

#diffuse modifactions on MCS (for .c, .h, .json)
./diffuse.sh $1

./Specialize_MCS.sh $1

touch ~/cookies/maj$1
