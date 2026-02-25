#!/bin/bash

# this script accept 4 parameter that is 
# - the SC ID Source (from 1 to N) - the SC ID of the source SAT  - $1
# - SC ID Destination - the SC ID of the new SAT                  - $2
# - IP Source - IP to be found and in the hierarchy and replaced by the IP Destination (IP of the source SAT) - $3
# - IP Destination - the IP of the VM - $4

#diffuse Sat modifactions on SAT X (for .c, .h, .json)
./diffuse.sh $4

#specialize the satellite
./Specialize_SAT.sh $1 $2 $3 $4


