#!/bin/bash
if [ $# -eq 0 ]; then
  echo "Error: no argument supplied, use 0 for help"
  exit 1
fi

if ! [[ $1 =~ ^[0-9]+$ ]]; then
    echo "Error: Argument is not an integer." >&2
    exit 1
elif [ "$1" -lt 0 ] || [ "$1" -gt 27 ]; then
    echo "Error: Argument must be between 0 and 27." >&2
    exit 1
fi

# this bash script is the master script to launch all attacks on CSS Platform (based on NOS3):

if [ $1 -eq 0 ]; then
  echo "****** LIST of EXPLOITS ********* "
  echo " 1  = KILL CI on SAT 1  "
  echo " 2  = KILL ISL on SAT 1  "
  echo " 3  = KILL MULTIPLE APP on SAT 1 "
  echo " 4  = KILL Front End "
  echo " 5  = KILL CAM (Optical Payload) "
  echo " 6  = Crash CRYPTOLIB On board SAT 1 "
  echo " 7  = SB Flooding "
  echo " 8  = CAM APP DELETE"
  echo " 9  = CAM DATA INTERCEPT "
  echo " 10 = CAM DATA CORRUPTION "
  echo " 11 = BATTERY SABOTAGE "
  echo " 12 = ATTITUDE SABOTAGE "
  echo " 13 = EVIL CAM APP FLOOD "
  echo " 14 = EVIL CAM APP KILL CI "
  echo " 15 = EVIL CAM APP DELETE SYSTEM"
  echo " 16 = Inversion of SCID (SABOTAGE) "
  echo " 17 = Duplication of SCID (SABOTAGE) "
  echo " 18 = ISL AUTOLOOP (ISL Crash) "
  echo " 19 = ISL MULTISAT LOOP (ISL Crash) "
  echo " 20 = Bypass SDLS (Encryption) "
  echo " 21 = Reset Crypto Key (HIJACKING) "
  echo " 22 = FULL CRYPTO HIJACK "
  echo " 23 = EVIL CAM APP KILL ISL (A2) "
  echo " 24 = EVIL CAM APP CAM CRASH (A5) "
  echo " 25 = EVIL CAM APP CAM DELETE (A8) "
  echo " 26 = EVIL CAM APP SAT SPIN (A12) "
  echo " 27 = DELETE ALL SYSTEM FILES (/cf) "
fi  

##########################################################################################################
#################################### SPACE SYSTEM ATTACKS ##############################################

######## A1 : CI KILL
######## Action: send TC to kill CI App for SAT 1 
######## GOAL : DoS Attack on SAT1 -> the satellite cannot communicate 

if [ $1 -eq 1 ]; then
  echo "*********************** KILL CI on SAT 1  ************************"
  python3 Attack_A1.py
fi

######## A2 : ISL KILL 
######## Action: send TC to kill ISL App for SAT 1 (NB: CI should be ok for this command to be executed)
######## GOAL : DoS Attack on Constellation -> Part of the Constellation cannot communicate 

if [ $1 -eq 2 ]; then
  echo "*********************** KILL ISL on SAT 1  ************************"
  python3 Attack_A2.py
fi

######## A3 : APPS KILL
######## Action: send TC to kill multiple Apps for SAT 1 
######## GOAL : DoS Attack on SAT 1 -> The  satellite is out of service --> NB: this can crash the simulator! (and the VM!!!)
######## NB : CI and ISL should be ok for this attack to be executed properly

if [ $1 -eq 3 ]; then
  echo "*********************** KILL MULTIPLE APP on SAT 1  ************************"
  python3 Attack_A3.py
fi


######## A4 : FE KILL
######## Action: kill FrontEnd 
######## GOAL : the constellation cannot communicate with the ground

if [ $1 -eq 4 ]; then
  echo "*********************** KILL Front End ************************"
  ./Attack_A4.sh
fi

######## A5 : CAM KILL  
######## Action: send TC to restart CAM application while the CAM is taking a photo ---> this will crash the FSW, but FSW will restart !
######## GOAL : sabotage of FSW

if [ $1 -eq 5 ]; then
  echo "*********************** CAM KILL ************************"
  python3 Attack_A5.py
fi

######## A6 : CRYPTO TC CRASH - https://visionspace.com/crashing-cryptolib/
######## Action: send TC to crash cryptolib on board (spi error) --> this will crash the FSW, but FSW will restart ! 
######## GOAL : segfault on cryptolib / sabotage FSW 
######## NB: Encryption can be activated on TC, but there is no encryption on TLM on ground (for now).
######## NOTE: CVE-2024-44911 related to out-of-bounds read vulnerability (no check for spi in [0,NUM_SA] before use!)
######## NOTE: after FSW restart the system is vulnerable to reply. 

if [ $1 -eq 6 ]; then
  echo "*********************** Crash CRYPTOLIB On board SAT 1 ************************"
  python3 Attack_A6.py
fi

######## A7 : TC SB FLOOD   
######## Action: send TC to continuously flood the SB 
######## --> ES app and ISL app will probably be squelched and the satellite is unable to process any other command
######## GOAL : SB flooding / DoS

if [ $1 -eq 7 ]; then
  echo "*********************** SB Flooding ***************************************"
  python3 Attack_A7.py
fi

######## A8 : APP DELETE  
######## Action: delete the CAM APP via TC plus the associated .so file in /cf.
######## GOAL : APP cannot be restarted. DoS on a specific APP.

if [ $1 -eq 8 ]; then
  echo "*********************** APP DELETE: CAM  ************************"
  ./Attack_A8.sh
fi

######## A9 : CAM DATA INTERCEPT  
######## Action: intercept the received tlm data for the arducam (CAM Payload). NB: On ground (MCS VM) image is stored in /tmp 
######## GOAL : Compromise Payload Data Confidentiality  

if [ $1 -eq 9 ]; then
  echo "*********************** CAM GET : CAM DATA INTERCEPT  ************************"
  source ./Attack_A9.sh 
fi

######## A10 : CAM DATA CORRUPTION  
######## Action: image data corruption (CAM Payload). NB: On board (SAT1) image is stored in /fsw/build/exe/cpu1/pic.jpg 
######## GOAL : CAM Payload Sabotage

if [ $1 -eq 10 ]; then
  echo "*********************** CAM DATA CORRUPTION  ************************"
  ./Attack_A10.sh
fi

######## A11 : EPS SABOTAGE   NB: EPS sim model has to be improved to better represent battery discharge! (battery discharge is not realistic)
######## Action: send TC to set all switch ON + point the SC opposite to the sun (this is for SAT2 in Sunsafe mode) -- NB: Do_Make_All to revert to normal 
######## GOAL : Discharge the battery / DoS / Integrity

if [ $1 -eq 11 ]; then
  echo "*********************** BATTERY SABOTAGE  **************************"
  echo "******** THE BATTERY VOLTAGE WILL decrease and SLOWLY DISCHARGE ! **"
  echo "******** THIS IS FOR SAT 2! ****************************************"
  echo "******** MEMO: Do_Make_All_VM to revert to normal ******************"
  ./Attack_A11.sh
fi

######## A12 : ADCS EVIL TC  
######## Action: send TC to compromize SAT attitude
######## GOAL : Attitude Sabotage / DoS

if [ $1 -eq 12 ]; then
  echo "*********************** ATTITUDE SABOTAGE  ************************" 
  python3 Attack_A12.py 
fi
 
######## A13 : EVIL CAM APP FLOOD  
######## this is replacing a .so file in fsw/build/exe/cpu1/cf
######## Action: Load a malicious CAM APP (.so) on the SAT 1 to flood the SB -- NB: Do_Make_All to revert to normal 
######## GOAL : SB Flooding (DoS on the Satellite bus)

if [ $1 -eq 13 ]; then
  echo "*********************** Load a malicious CAM APP on the SAT  *****************************"
  echo "*********************** SB FLOODING when TC CAM_EXP3_CC   ********************************"
  echo "*********************** ./Do_Make_All to revert to normal behavior ! ************************"
  #./Attack_A13.sh
  ./Attack_Cam.sh arducam_A13_floodhk.so
fi

######## A14   : EVIL CAM APP KILL CI 
######## Action: send a TC from evil CAM APP to KILL CI 
######## GOAL : use a malicious app to KILL CI, DoS.

if [ $1 -eq 14 ]; then
  echo "*********************** KILL CI from EVIL CAM APP  ************************"
  #./Attack_A14.sh 
  ./Attack_Cam.sh arducam_A1.so
fi

######## A15 : EVIL CAM APP DELETE.
######## Action:  delete all in /cf from evil CAM APP.
######## GOAL : DoS, Sabotage

if [ $1 -eq 15 ]; then
  echo "*********************** EVIL CAM APP Delete all in /cf  ************************"
  #./Attack_A15.sh 
  if [ "$#" -eq 2 ]; then
    echo "path : $2"
    printf "$2" > /tmp/target_folder.txt
  else
    rm /tmp/target_folder.txt 2>/dev/null
  fi
  ./Attack_Cam.sh arducam_A15_customizable.so
fi

##########################################################################################################
#################################### CONSTELLATION ATTACKS ###############################################

######## A16 : SCID FE INV  
######## Action: Inversion of SCID in FrontEnd TC (between SAT1 and SAT2, need to recompute FECF!)
######## GOAL : Constellation Sabotage 

if [ $1 -eq 16 ]; then
  echo "*********************** Inversion of SCID in FrontEnd ****************"
  echo "*********************** NB: SAT 2 is needed ! ************************"
  ./Attack_A16.sh
fi

######## A17 : SCID FE DOUBLE
######## Action: Duplication of SCID in FrontEnd TC (only TC for SAT 1 are duplicated to SAT 2, need to recompute FECF!)
######## GOAL : Constellation Sabotage 

if [ $1 -eq 17 ]; then
  echo "*********************** Duplication of SCID in FrontEnd **************"
  echo "*********************** NB: SAT 2 is needed ! ************************"
  ./Attack_A17.sh
fi

######## A18 : ISL AUTOLOOP (SAT1) 
######## Action: modify the ISL route table via TC to induce autoloop on SAT1
######## GOAL : crash ISL for SAT1
######## NB: ISL_TTL_FEATURE should be 0 in isl_app.h (no TTL) for this attack to work

if [ $1 -eq 18 ]; then
  echo "*********************** ISL AUTOLOOP  ************************"
  echo "***************** This will crash ISL on SAT1 ****************"
  echo "***************** NB: ISL_TTL_FEATURE should be 0 ************"
  echo "***************** NB: deactivate LOOP_CHECK  *****************"
  python3 Attack_A18.py
fi

######## A19 : ISL LOOP MULTISAT (SAT1-SAT2) 
######## Action: modify the ISL route table via TC to induce multi satellite loop 
######## GOAL : crash ISL for multiple SAT
######## NB: ISL_TTL_FEATURE should be 0 in isl_app.h (no TTL) for this attack to work

if [ $1 -eq 19 ]; then
  echo "***************** ISL MULTISAT LOOP  *************************"
  echo "***************** This will crash ISL ************************"
  echo "***************** NB: SAT 2 and 3 are needed ! ***************"
  echo "***************** NB: ISL_TTL_FEATURE should be 0 ************"
  echo "***************** NB: deactivate LOOP_CHECK  *****************"
  python3 Attack_A19.py
fi

##########################################################################################################
#################################### SPACE SYSTEM ATTACKS ##############################################


######## A20 : BYPASS CRYPTO - https://securitybynature.fr/post/hacking-cryptolib/
######## Action: bypass SDLS (encryption) using cryptolib vulnerabilities
######## GOAL : take over (send TC in clear mode even if encryption is activated!)

if [ $1 -eq 20 ]; then
  echo "*********************** BYPASS the CRYPTOLIB ! ************************"
  echo "*********************** SEND SPP in CLEAR MODE (CAM NOOP) *************"
  echo "*********************** even if Encrytpion ON *************************"
  python3 Attack_A20.py 
fi

######## A21 : CRYPTO HIJACK - https://securitybynature.fr/post/hacking-cryptolib/
######## Action: Hijaking using cryptolib vulnerabilities
######## GOAL : Hijacking demo

if [ $1 -eq 21 ]; then
  echo "*********************** Hijacking DEMO using OTAR PDU Command **************"
  echo "*********************** and CryptoLib Vulnerability ************************"
  echo "*********************** RESET CRYPTO KEY only for SPI=44  ******************"
  python3 Attack_A21.py
fi

######## A22 : FULL CRYPTO HIJACK - https://securitybynature.fr/post/hacking-cryptolib/
######## Action: Hijaking using cryptolib vulnerabilities
######## GOAL : Hijacking the satellite!

if [ $1 -eq 22 ]; then
  echo "*********************** Hijacking using OTAR PDU Command *******************"
  echo "*********************** and CryptoLib Vulnerability ************************"
  echo "*********************** RESET CRYPTO KEY for all VC!  **********************"
  echo "*********************** Take control of the space link! ********************"
  python3 Attack_A22.py
fi

if [ $1 -eq 23 ]; then
  echo "*********************** KILL ISL from EVIL CAM APP  ************************"
  #./Attack_A14_A2.sh
  ./Attack_Cam.sh arducam_A2.so
fi

if [ $1 -eq 24 ]; then
  echo "*********************** CAM CRASH from EVIL CAM APP  ************************"
  #./Attack_A14_A5.sh
  ./Attack_Cam.sh arducam_A5.so
fi

if [ $1 -eq 25 ]; then
  echo "*********************** CAM DELETE from EVIL CAM APP  ************************"
  #./Attack_A14_A8.sh
  if [ "$#" -eq 3 ]; then
    echo "file : $2"
    echo "app : $3"
    printf "$2" > /tmp/target_path.txt
    printf "$3" > /tmp/target_app.txt
  else
    rm /tmp/target_path.txt /tmp/target_app.txt 2>/dev/null
  fi
  ./Attack_Cam.sh arducam_A8_customizable.so
fi

if [ $1 -eq 26 ]; then
  echo "*********************** SAT SPIN from EVIL CAM APP  ************************"
  #./Attack_A14_A12.sh
  ./Attack_Cam.sh arducam_A12.so
fi

if [ $1 -eq 27 ]; then
  echo "*********************** DELETE /CF from ground  ************************"
  ./Attack_A27.sh
fi
