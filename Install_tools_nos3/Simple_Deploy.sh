#!/bin/bash

echo "#################### Adapt Address #################"
./Do_Adapt_Address.sh 
echo "#################### Adapt NBSAT #################"
./Do_Adapt_NbSAT.sh
echo "#################### Specialize VM #################"
./Do_Specialize_All_VM.sh
echo "#################### Update VM #################"
./Do_Update_All.sh
echo "#################### Make Clean #################"
./Do_MakeClean_All_VM.sh
echo "#################### Make All ################"
./Do_Make_All_VM.sh
echo "#################### Now you can go on VM MCS #################"
