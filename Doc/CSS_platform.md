# CSS Platform Architecture

*Last Updated: 12/2024*

## Platform Overview

The CSS Platform is composed of multiple repositories designed to work together as an integrated space system simulator.

## Repository Structure

| Path | Purpose |
|------|---------|
| `/home/nos3/Install_tools_nos3` | Deploy a constellation and diffuse updates (see Procedure.txt) |
| `/home/nos3/CSS_Attacks` | Folder containing the exploits (CSS project) |
| `/home/nos3/Input_Generator` | Input generator |
| `/home/nos3/Desktop/github-nos3` | NOS3 code and modifications |
| `/home/nos3/eclipse-workspace/frontend` | FrontEnd component code |
| `/home/nos3/eclipse-workspace/imager` | Imager component code |
| `/home/nos3/eclipse-workspace/mission` | Mission component code |
| `/home/nos3/Scenario_Manager` | Scenario manager code |
| `/opt/nos3/` | NASA 42 and Cosmos code |
| `/home/nos3/cookies` | Cookie storage |

### Example Commands

```bash
# Deploy and update constellation
cd /home/nos3/Install_tools_nos3
./Do_Update_All.sh
```
```bash
# Execute attacks
cd /home/nos3/CSS_Attacks
./Master_Attacks_Scripts.sh 1
```

## Known Limitations & Disclaimers

### Not Currently Available or Working

- **CFDP**: Not working properly (COSMOS + CF CFS) → external library used instead
- **COP-1**: Only partial implementation available
- **Encrypted Telemetry**: Not working (debugging in progress)
- **TLM ADCS**: Inactive by default to limit message volume
- **CryptoLib**: Not updated to preserve existing vulnerabilities for testing

### Simulation Notes

> ⚠️ **The simulation is NOT real-time.** The simulation runs slower than real-time. Check Sim Time in the 42 window to estimate the ratio.

## System Configuration

### Step Time Configuration

To change the simulation step time, modify the following files:

| File | Parameter | Default | Note |
|------|-----------|---------|------|
| `Inp_Sim.txt` | Step time | 0.01 | Base simulation step |
| `SC_NOS3.txt` | Sample time | ≥ Inp_Sim.txt | All components must match |
| `nos3_simulator.xml` | Line 11/12 | 10000 | Timing multiplier |
| `cfe_psp_start.c` | Line 88 | 100 | Set to 0.01s step |

## Troubleshooting

### FSW Not Launching at Startup
**Solution:** Restart the VM.

### Socket Communication Issues
**Symptoms:** NaN values or freeze between satellite and 42
**Solution:** Restart the simulator.

### GPS Data Issues
**Symptoms:** HK HEADER + TRAILER FOUND message from FSW
**Solution:** Check GPS data configuration.

### Data Freezing
**Symptoms:** Data from 42 to satellite freezes intermittently
**Solution:** Relaunch the simulation.

## Important Data Locations

All interesting data are stored in the `/tmp` folder:

### MCS VM (`/tmp`)
- Decoded photos captured during mission

### SAT VM (`/tmp`)
- Input file for imager
- IDS detection logs
- ADCS control errors (normal mode)
- Visibility information (satellite wrt ground station defined in `isl_app.h`)

## Critical Telecommands

The following commands are treated as **critical TCs** and include MAC authentication at the end of SPP packets:

```
CFE_ES_STOP_APP
GENERIC_ADCS_SET_MODE_CC
```

> **Note:** MAC is set to `1234` for prototyping purposes only.


## Security & Intrusion Detection

### IDS/IPS Status
- **Active by default**
- Configuration: See `ids_probes.h`

### Attack Packet Labeling
CSS Attack packets are labeled differently from normal packets for detection performance analysis. In particular, 8th octet of SPP is set to 0x02 instead of 0x00 OR 6th bit of 1st octet of TF header is set to 1 instead of 0 (ex. in hex 0x20 becomes 0x24).

## CFDP Implementation

### External Library
CFDP is implemented using an external Python library:
- **Location:** https://gitlab.com/librecube/lib/python-cfdp
- **Filestore:** `/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf`
- **Supported Classes:** Class 1 only (Class 2 to be extended)

### CFDP Commands
```
CFS CF_UPLINK_FILE    (MsgId: 0x18B3)
CFS CF_DOWNLINK_FILE  (MsgId: 0x18B6)
```

### CFDP Uplink Flow
```
COSMOS → Cryptolib → External CFDP Library (/cf of MCS) 
→ Send to CryptoLib → FrontEnd → ISL → CI → BUS → CF 
→ External CFDP Server → save in /cf of SATX
```

### CFDP Downlink Flow
```
COSMOS → Cryptolib → FrontEnd → ISL → CI → BUS → CF 
→ External CFDP Library (/cf of SATX) → send to CF → BUS → TO 
→ ISL → FrontEnd → Cryptolib → External CFDP Server 
→ save in /cf of MCS
```

### CFDP Command Example

```
COMMAND CF CF_TX_FILE <%= CosmosCfsConfig::PROCESSOR_ENDIAN %> 
"This command (theoretically) causes a CFDP Put Request to be issued from FSW to the ground."
  APPEND_ID_PARAMETER CCSDS_STREAMID 16 UINT MIN_UINT16 MAX_UINT16 0x18B3 
    "CCSDS Packet Identification" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_SEQUENCE 16 UINT MIN_UINT16 MAX_UINT16 0xC000 
    "CCSDS Packet Sequence Control" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_LENGTH 16 UINT MIN_UINT16 MAX_UINT16 137 
    "CCSDS Packet Data Length" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_FC 8 UINT MIN_UINT8 MAX_UINT8 2 
    "CCSDS Command Function Code"
  APPEND_PARAMETER CCSDS_CHECKSUM 8 UINT MIN_UINT8 MAX_UINT8 0 
    "CCSDS Command Checksum"
  APPEND_PARAMETER CLASS 8 UINT 0 1 0 "CFDP Class 1 or 2"
    STATE "CLASS 1 - No Feedback" 0
    STATE "CLASS 2 - With Feedback" 1
  APPEND_PARAMETER KEEP 8 UINT 0 1 1 "Keep or delete file after transfer"
    STATE "Delete" 0
    STATE "Keep" 1
  APPEND_PARAMETER CHAN 8 UINT 0 1 0 "CFDP Chan 0 or 1"
    STATE "Chan 0" 0
    STATE "Chan 1" 1
  APPEND_PARAMETER PRIORITY 8 UINT 0 255 1 "Playback Priority"
  APPEND_PARAMETER DEST_ID 32 STRING "0.21" "Destination ID - 2 bytes"
  APPEND_PARAMETER SRCFILENAME 512 STRING "/data/" "Path/Filename to send"
  APPEND_PARAMETER DSTFILENAME 512 STRING "/tmp/data" "Path/Filename at destination"
```

## Advanced Topics

### Geometric Visibility Computation

Geometric visibility is computed by ISL when GPS data are available:
- **Prerequisite:** NOVATEL activation via TC
- **Data Transfer:** Results sent to MCS VM in `/tmp` folder as `visiX.txt` files
- **Purpose:** Provides satellite visibility to ground station

> **Note:** This is a workaround due to lack of direct communication between MCS VM and NASA 42 VM. Future releases may improve this.

### Dynamic Routing

Dynamic routing updates are managed via FrontEnd component:
- **File:** `FE_routegen.cpp`
- **Method:** Creates uplink command for cryptolib

### Cryptolib Integration

- **Standalone Version:** Compiled at launch on MCS VM
- **Modifications:** For Cryptolib changes, check startup on MCS VM

### Packet Length Calculation

CCSDS length is in bytes and counts all bytes **AFTER** the FC (Function Code).

**Example:**
```
137 × 8 = 8 + 8 + 8 + 8 + 8 + 32 + 512 + 512 = 1096 bytes
```

### Socket Initialization

For 42 socket initialization details, see:
- `iokit.c`
- `42ipc.c`

Located in: `/opt/nos3/42`


## Mission Configuration

### Automatic Mission

The automatic mission is defined in the Mission component and starts a few minutes after simulator launch.

**Target Configuration File:**
```
/home/nos3/eclipse-workspace/mission/config/targets.txt
```

**Mission Features:**
- Photo capture targets
- TO Telemetry activation
- Sensors activation
- Normal mode activation (via TC during mission)

### Time Adjustment

To change the starting time:
```bash
./diffuse_nos3_time.sh <delta_in_seconds>
```

**Note:** Absolute start time in `nos3-simulator.xml` is in J2000 format.

### Ground Stations

Ground stations used for visibility computation are defined in:
```
Inp_Sim.txt
```

## References & Architecture Details

- **NASA 42 & COSMOS:** Located in `/opt/nos3/`
- **Socket Communication:** See `iokit.c` and `42ipc.c` in `/opt/nos3/42`
- **IDS Configuration:** See `ids_probes.h`
- **Visibility Computation:** Handled by ISL with GPS data from NOVATEL


*For additional information, refer to the individual component documentation and README files in each repository.*
