### 

To test exploits simply use 

./Master_Attacks_Scripts.sh + the number of the exploit you want to test 

./Master_Attacks_Scripts.sh 0   is for HELP. 


NB: Important notes when launching attacks.

- some attacks can crash the simulation or the VM ! (Ex. 3) 

- some attacks should be launched during the satellite mission (Ex. 22)

- some attacks requires multiple satellites in the simulation (Ex. 19)

- often (but not always) the threat model is based on the hp that attackers have taken control of the MCS, thus malicious TCs are sent to FrontEnd or Standalone CryptoLib.

- to change the target of the attack (satellite 1 , 2 , etc) it is important to check the input port of the TC in the attack. If input is Cryptolib Standalone (SPP packet) then adapt the input port. if input is FrontEnd then adapt the SCID into the Transfer Frame Header (usually the second octet).

- some attacks are not fully realisitic, but useful for demo or prototyping (A9, A10, ...).

- the simulator should be improved sometimes for the attack consequences to be realistic (Ex. A11: battery model should be improved, switch are not connected, etc... Ex. A12: satellite TC/TM should be impacted by loss of control, ...)

- please note that the simulator does not include a FDIR component ! In reality a FDIR component could detect something and it could change the result of the attacks.

- please note that some mitigation means are active by default to detect or block some of the attacks. You have to deactivate some protections to fully test some of the attacks (see README in home). 






