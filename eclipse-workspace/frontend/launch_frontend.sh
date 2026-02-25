#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd $DIR
./Release/frontend config/netConfig.txt config/routeConfig.txt config/flagsConfig.txt&
