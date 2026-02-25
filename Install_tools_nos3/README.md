# Simulator Deployment Guide

## Overview

This guide explains how to deploy the simulator from **one generic VM** after all required virtual machines (VMs) have been instantiated on the same network.

> **Default IP Addresses**
>
> - MCS = `192.168.100.5`
> - NASA 42 = `192.168.100.42`
> - SAT1 = `192.168.100.11`
> - SAT2 = `192.168.100.12`
> - etc.

---

## Prerequisites

- All required VMs are instantiated.
- All VMs are connected to the same network.
- You have SSH access to all machines.
- Required scripts are present on the respective VMs.

---

## Deployment Steps

### 0 - Connectivity Check (from SAT1)

From **SAT1 VM**:

- Verify you can `ping` all other VMs.
- Verify you can `ssh` into all other VMs.

Example:

```bash
ping 192.168.100.5
ssh nos3@192.168.100.5
```

---

### 1 - Configure Number of Satellites

On **SAT1 VM**:

- Edit the configuration file:

```bash
vi IP_Scenarios.sh
```

- Modify the `NBSAT` variable as needed.

---

### 2 - Run Deployment Script

On **SAT1 VM**:

```bash
./Simple_Deploy.sh
```

Wait until the process completes successfully.

---

### 3 - Install Tools and Build (on MCS)

Log into **MCS VM**.

Open a terminal **as root**, then:

```bash
cd Install_tools_nos3
./launch_containers.sh
```

After containers are launched (still running as `root`):

```bash
cd github-nos3
make
```

---

### 4 - Start / Stop the Simulator

On **SAT1 VM**, inside `github-nos3`:

Start the simulator:

```bash
./start.sh
```

Stop the simulator:

```bash
./stop.sh
```

---

## Development Workflow

If you need to modify or develop the code:

1. Perform development on **SAT1 VM**.
2. Diffuse (propagate) changes to all VMs using:

```bash
./Do_Update_All.sh
./Do_Make_All_VM.sh
```

---

## Summary

| Step | Machine | Action |
|------|---------|--------|
| 0 | SAT1 | Check ping + SSH |
| 1 | SAT1 | Configure `IP_Scenarios.sh` |
| 2 | SAT1 | Run `Simple_Deploy.sh` |
| 3 | MCS | Launch containers + `make` |
| 4 | SAT1 | Start/Stop simulator |

---

## Notes

- All VMs must be on the same network.
- Ensure SSH access is properly configured.
- Always wait for deployment scripts to fully complete before proceeding.

---

**Deployment complete. 🚀**
