# SmallSat COSMOS Database
This repository is to be used to capture command and telemetry definitions, test procedures / scripts, and operational scripts.

## Launching a Specific Configuration

From a terminal in this directory you have the following configuration options:
* Baseline (all targets)
  - `ruby Launcher`
* NOS3
  - `export MISSION_NAME=$(echo "NOS3")`
  - `export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")`
  - `ruby Launcher -c nos3_launcher.txt --system nos3_system.txt`

If the `PROCESSOR_ENDIANNESS` environmental variable is not explicitly set, it will default to `"LITTLE_ENDIAN"`.

## Adding Components / Targets 
Each component should have it's own target directory.
For example, the sample component has the directory `./config/targets/SAMPLE`.
The component target directory must be uppercase.
This directory minimally contains:
* `cmd_tlm` directory
  - Text files containing the command and telemetry definitions
  - May be a single file or multiple files, recommend one for command and one for telemetry
* `target.txt` file
  - List of ignored parameters that should not be shown
  - Parameters are still interpreted by COSMOS and stored in the data archives

After the component files exist, it will need to be added to the following:
* `./config/system/MISSION_system.txt`
* `./config/tools/cmd_tlm_server/cmd_tlm_server.txt`
* `./config/tools/cmd_tlm_server/MISSION_cmd_tlm_server.txt`

## Specific Mission Configurations
Each mission configuration minimally requires the following files:
* `./config/system/MISSION_system.txt`
* `./config/tools/cmd_tlm_server/MISSION_cmd_tlm_server.txt`
* `./config/tools/launcher/MISSION_cmd_tlm_server.txt`

## References
* [Ball Aerospace COSMOS documentation](https://cosmosrb.com/docs/home/)
