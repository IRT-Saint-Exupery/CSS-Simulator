# Input Generator – Usage

## How to Use the Input Generator

### Generate a scenario file

```bash
python3 Main.py create scenarios/test.json
```

This generates a `.json` file corresponding to the desired commands.  
Follow the instructions displayed in the terminal to select the command.

---

### Modify the scenario file (if needed)

You can manually edit the generated `.json` file to adjust the default parameters.

---

### Send the commands

```bash
python3 Main.py send scenarios/test.json
```

This sends the commands to the Cryptolib input port.

---

## Optional: Specify a port

You can use the `--port` option to specify a custom port.

Example:

```bash
python3 Main.py send scenarios/test.json --port 6012
```

This sends the commands to the Cryptolib `sat2` input port `6012`.

---

## Additional Information

- Must be used on the VM representing the MCS.
- Fuzzing and timestamp fields development is in progress. These features are not fully available yet.
- You can create combinations of commands directly with the `create` argument or by using a dedicated script.

Example of a dedicated script:

```bash
python3 Init_mission.py 6010
```

This sends a predefined series of commands to port `6010`.
