# CSS Network Configuration and Utilities

This document contains helper commands and utilities for configuring network settings and capturing network traffic in the CSS simulator environment.

## Network Configuration

These commands set up network interface configuration for the simulation environment, assigning IP addresses and routing rules.

### Configure Network Interface

```bash
sudo ip addr add 192.168.100.11 dev eth0
sudo ip route add default via 192.168.100.11 dev eth0
ip link dev eth0 up
```

**What it does:**
- `sudo ip addr add 192.168.100.11 dev eth0` - Assigns the IP address `192.168.100.11` to the `eth0` network interface
- `sudo ip route add default via 192.168.100.11 dev eth0` - Sets the default gateway route through `192.168.100.11` on `eth0`
- `ip link dev eth0 up` - Brings up the `eth0` network interface (makes it active)

## Network Address Replacement

### Find and Replace Incorrect IP Address

```bash
find -type f -exec sed -i 's/192.168.100.4/192.168.100.11/g' {} \;
```

**What it does:**
- Searches recursively through all files in the current directory
- Uses `sed` to find all occurrences of the incorrect IP address `192.168.100.4`
- Replaces them with the correct IP address `192.168.100.11`
- `-i` flag modifies files in-place
- Useful for correcting configuration files after network setup changes


## Network Traffic Capture

Capture Network Packets with tcpdump

```bash
sudo -s
tcpdump -i eth0 udp -w Capture2811204.pcap
```

**What it does:**
- `sudo -s` - Starts an interactive shell with root privileges
- `tcpdump -i eth0 udp -w Capture2811204.pcap` - Captures UDP traffic on the `eth0` interface
  - `-i eth0` - Specifies the network interface to capture from
  - `udp` - Filters to only capture UDP packets
  - `-w Capture2811204.pcap` - Writes the captured packets to a PCAP (Packet Capture) file for later analysis

**Use case:** Used for network traffic analysis and anomaly detection in security testing scenarios.

## Reference Configuration

**Target IP Address:** `192.168.100.11`
