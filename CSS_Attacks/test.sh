#!/bin/bash

nb=$RANDOM
MOD=10

let "nb%=$MOD"

echo "$nb"

let "nb+=1"

sleep "${nb}s"

echo "TEST $nb"
