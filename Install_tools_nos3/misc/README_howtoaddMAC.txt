How we have added the MAC in CFE_ES_STOP_APP : 

1 - add append_parameter MAC in /home/nos3/Desktop/github-nos3/gsw/cosmos/config/targets/CFS/cmd_tlm/ES.txt for CFE_ES_STOP_MAC
2 - adapt also CCSDS_LENGTH to inlcude MAC length
3 - adapt the command structure in /home/nos3/Desktop/github-nos3/fsw/cfe/modules/es/fsw/inc/cfe_es_msg.h

NB: parameters as CFE_MISSION_MAX_API_LEN are in /home/nos3/Desktop/github-nos3/fsw/nos3_defs/nos3_mission_cfg.h 

NB: if parameter should be defined with no user input, use the ignore statement in /home/nos3/Desktop/github-nos3/gsw/cosmos/config/targets/CFS/targets.txt
