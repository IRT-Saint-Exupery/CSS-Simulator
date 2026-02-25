#!/bin/bash

# IP address for the destination of the diffuse
cd ~/Desktop/github-nos3
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd ~/eclipse-workspace/frontend
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd ~/eclipse-workspace/mission
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd ~/eclipse-workspace/imager
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd ~/Input_Generator
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd ~/CSS_Attacks
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd /opt/nos3
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd /home/nos3/Install_tools_nos3
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

cd /home/nos3/Scenario_Manager
find . \( -name '*build*' -o -name 'Makefile' -o -name '.git' \) -prune -o \( -newer ~/cookies/maj$1 -exec /home/nos3/Install_tools_nos3/diffusefile.sh {} $1 \; \)

touch ~/cookies/maj$1
