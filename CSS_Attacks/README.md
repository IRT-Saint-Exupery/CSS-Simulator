# Exploit Testing – Usage Guide

## How to Test Exploits

To test exploits, use:

```bash
./Master_Attacks_Scripts.sh <exploit_number>
```

Example:

```bash
./Master_Attacks_Scripts.sh 0
```

`0` displays the HELP menu.

---

## Important Notes When Launching Attacks

- Some attacks can crash the simulation or the VM (e.g., 3).
- Some attacks must be launched during the satellite mission (e.g., 22).
- Some attacks require multiple satellites in the simulation (e.g., 19).
- In most cases (but not always), the threat model assumes attackers have taken control of the MCS. Malicious TCs are therefore sent to the FrontEnd or Standalone CryptoLib.
- To change the target of the attack (satellite 1, 2, etc.), check the TC input port used in the attack:
  - If the input is **Cryptolib Standalone (SPP packet)**, adapt the input port.
  - If the input is **FrontEnd**, adapt the SCID in the Transfer Frame Header (usually the second octet).
- Some attacks are not fully realistic but are useful for demonstrations or prototyping (e.g., A9, A10, ...).
- The simulator may need improvements for some attack consequences to be realistic:
  - Example A11: battery model should be improved, switches are not connected, etc.
  - Example A12: satellite TC/TM should be impacted by loss of control.
- The simulator does **not** include an FDIR component. In reality, an FDIR component could detect anomalies and change the outcome of attacks.
- Some mitigation mechanisms are active by default to detect or block certain attacks. You may need to deactivate protections to fully test some attacks (see README in home).
