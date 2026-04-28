import ScenarioManager
import CmdParser
import argparse
import os
import json

#for now only support for 1 command at a time

def args_init():
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help=".json file")
    parser.add_argument("--command_nb", type=int, default=1)
    parser.add_argument("json_field", help="field name")
    parser.add_argument("--type", choices=['INT','STRING'], default="INT", help="field name")
    parser.add_argument("value", help="value to put in field")
    args = parser.parse_args()
    return args

args = args_init()

path = args.filepath

command_nb = args.command_nb
field = args.json_field
field_type = args.type
val = args.value

if os.path.exists(os.path.join(path)):
    with open(path, "r") as f:
        data = json.load(f)

    cmd_str = f"command{command_nb}"
    command_name = ([l for l in list(data[cmd_str].keys()) if ((l != "TIMESTAMP") & (l != "TARGET") & (l != "FUZZ"))])
    if (len(command_name) > 1):
        print("too much command names, bad json format detected !")
        exit(1)
    else:
        command_name = command_name[0] #keep the only element and de-list it

    # Modify the latitude and longitude fields
    if (field_type == "INT"):
        data[cmd_str][command_name][field] = val
    else:
        data[cmd_str][command_name][field] = '"'+val+'"'

    # Write the updated data back to the same file (or a new file if preferred)
    with open(path, "w") as f:
        json.dump(data, f, indent=4)
else:
    print("Path error - file does not exist !")
