<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
<!--[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]-->



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="logo.png" alt="Logo" width="150" height="150">
  </a>

<h3 align="center">CSS Project</h3>

  <p align="center">
    Cybersecurity for Space Systems (CSS)
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#publications">Publications</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#disclaimer">Disclaimer</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

CSS project (2023 - 2026) is led by the French Institute for Technological Research ([IRT](https://www.irt-saintexupery.com/)) in Toulouse. One of the goal of CSS project is to build a state-of-the-art Space System Simulation platform for Cybersecurty R&D. The CSS simulator can be used to deploy a constellation of satellites, including Flight Software, Mission Control System, space environment and space link communications (based on CCSDS stack).

The simulator has been developed based on NASA [NOS3](https://github.com/nasa/nos3/releases/tag/v1_06_02) version 1.6.2. (released 07/08/2023). 

The reference space system used in the simulator (with some modifications) is [STF-1](https://www.nasa.gov/simulation-to-flight-1/) mission. 

NASA NOS3 simulator gathers multiple open source projects such as
- [NASA CFS](https://github.com/nasa/cFS) (Flight Software)
- [COSMOS](https://github.com/OpenC3/cosmos) (Mission Control System)
- [NASA 42](https://github.com/ericstoneking/42) (Flight dynamics and visualization)
- [CryptoLib](https://github.com/nasa/CryptoLib) (SDLS)

To create a single multi-purpose, modular and highly customizable space system simulator (see [NOS3](https://github.com/nasa/nos3)).

In the context of CSS project, it has been decided to split the simulator on multiple VM, in order to increase the modularity and to have the possibility to have different operators on different VMs. In particular, it has been chosen to use
- 1 VM for each satellite (NASA CFS).
- 1 VM for the Mission Control System (MCS, ground segment based on COSMOS).
- 1 VM for the flight dynamics and visualization part (NASA 42).


<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="CSS_Architecture.jpg" alt="Logo" width="750" height="500">
  </a>


<div align="left">
  </a>

<div style="text-align: left;">

<!-- GETTING STARTED -->
## Getting Started

Please note that CSS Platform is composed by multiple repositories :

1. */home/nos3/Install_tools_nos3* : this is to deploy a constellation and to diffuse updates (see *Procedure.txt* for all the detailed steps). Example to diffuse updates to all satellites: *./Do_Update_All.sh*
2. */home/nos3/CSS_Attacks* : this is the folder containing the exploits developed for CSS project. For help *./Master_Attacks_Scripts.sh 0*
3. */home/nos3/Input_Generator* : the python code of the input generator is here. 
4. */home/nos3/Desktop/github-nos3* : the modified NOS3 code is here (including ISL and IDS/IPS). Example to launch the simulator : *./start.sh* (and *./stop.sh* to stop the simulator).
5. */home/nos3/eclipse-workspace/frontend* : the FrontEnd component code is here. 
6. */home/nos3/eclipse-workspace/imager* : the imager component code is here. 
7. */home/nos3/eclipse-workspace/mission* : the automatic mission component code is here. 
8. */home/nos3/Scenario_Manager* : the code of the scenario manager is here (launch a sequence of simulations and save the data). 
9. */opt/nos3/* : NASA 42 and COSMOS code are here (included in the project VM). Cosmos folders are duplicated for each satellite in the constellation (cosmos1, cosmos2, etc).
10. */Dataset_Utils/*: code for automatic dataset generation (included as external repository).

Limitations:
 
1. The simulator architecture is adapted only for small constellations. 
2. The RF layer is not simulated.

### Prerequisites

The CSS Simulator is based on multiple VMs: 1 VM for the Mission Control System (COSMOS), 1 VM for Flight Dynamics and Visualization (NASA 42), 1 VM for each satellite in the constellation (NASA cFS).

The "comfortable" ressources CPU/RAM(GB) required to run the simulator can be summarized as follow :
1. Satellite VM : 2CPU / 3GB or higher.
2. Visualization VM : 4CPU / 4GB or higher.
3. MCS VM: 1CPU / 4GB for each satellite to be controlled (or higher). 

The VMs should share the same network (for deployment). The VMs are all defined from a generic VM (based on Ubuntu 20.04 LTS, same as NOS3 VM) that can be specialized after creation.

The preferred method to deploy CSS simulator is via the main generic VM of the project, that you can request here : [WORK IN PROGRESS]().

The CSS simulator has been defined and deployed in a proprietary cyber range environment (**CITEF** by [Nexova](https://www.nexovagroup.eu/en)). 
However, a priori it is possible to deploy and test the simulator locally (or on dedicated hardware) using VirtualBox (as for NOS3), see next section.


### Installation

For deployment and installation, once you have downloaded the main VM, there are two options.

1. You have access to **CITEF** (or similar software) and you have created your scenario and the associated network. For example the following scenario : 

<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="scenario1.png" alt="Logo" width="350" height="200">
  </a>

<div align="left">

  In this case you can simply log into the VM representing the first satellite of your constellation (SAT1) and you can follow the steps described in the *Procedure.txt* file in */home/nos3/Install_tools_nos3*. 
  
  - modify *IP_Scenarios.sh* to choose *NBSAT* (number of satellites).
  - call *./Simple_Deploy.sh* and wait for the script to complete (update and specialize the VMs).
  - log into VM MCS, open a terminal as root, call *launch_containers.sh*, then in *github-nos3* folder call *make*.

2. You have access to Virtual Box (or similar software) and you have created your scenario and the associated network manually. In this case, once all VMs are launched, you can simply log into the VM representing the first satellite of your constellation (SAT1) and you can follow the steps described in the *Procedure.txt* file in */home/nos3/Install_tools_nos3*. 

  - WORK IN PROGRESS

Here an example of CITEF scenario with 7 satellites : 

<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="scenario7.png" alt="Logo" width="600" height="400">
  </a>
  
<div align="left">

<!-- USAGE EXAMPLES -->

## Usage

To **launch the simulator** just log to the first satellite (ex. *SAT1(.11)*) and use *./start.sh*  (and *./stop.sh* to stop the simulator) in *github-nos3* folder.

After launch (generally 1 to 2 minutes) : 
1. NOS3 flight software tabs will be available in each SAT VM. 
2. COSMOS GUI will be available in MCS VM. A web tab will be available for each SAT in Firefox. Moreover, the operator will have access to CryptoLib Standalone tabs and FrontEnd tab.
3. CFDP servers tabs will be available in MCS VM and in each VM SAT.
4. Space environment simulation (NASA 42) will be available in VM VISU (see image below).
5. The camera imager (what the Arducam can see) will be available in each SAT (NB: Imager is updated only if SAT is in "Normal Mode"). 

<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="42Cam_7Sat.png" alt="Logo" width="1100" height="400">
  </a>

<div align="left">

The **automatic mission** defined in mission component will start a few minutes after starting the simulator. In particular, the targets of the mission are defined in *~/eclipse-workspace/mission/config/targets.txt*. TO Telemetry, Sensors and Normal mode are activated by TC during the mission. You can also craft your own mission/test using the scriptrunner menu of COSMOS.
 
You can test some **attacks** using:
 - *./Master_Attacks_Scripts.sh 0* for help.
 - *./Master_Attacks_Scripts.sh* \< *number of the exploit* \>

See *CSS_Attacks* folder and CSS manual for more details on the attacks. 

A live demonstration of CSS simulation platform has been presented at:

* [CYSAT 2025](https://cysat.eu/cysat-europe)
* [Starion/Nexova Tech Talks](https://youtu.be/7SkSOk2pjhM?feature=shared)
* [WORK IN PROGRESS]()


**Notes and Limitations:**
1. IDS/IPS component is active by default, you should modify the script */github-nos3/components/ids/fsw/src/ids_probes.h* to disable the 3 probes and recompile the simulator. The IDS is in mode "prevention" by default once activated. Logs about attacks are dropped in */tmp* folder. 
2. TTL (Time To Leave) information is added by default in front of each TF (1 octet) in order to avoid loops into the constellation network. A counter about dropped packets is available in ISL Telemetry. In order to deactivate TTL check in, one can use *ISL_TTL_FEATURE=0* in *isl_app.h*. Warning: *ISL_TTL_INIT* in ISL and *TTL_INIT* in FrontEnd shall be the same. 
3. A check for new routing tables received by TC is added by default in ISL to avoid loops. Comment the *#define ISL_LOOP_CHECK* in *isl_app.h* to deactivate this feature. 
4. A dummy MAC is introduced to identify some Critical TCs as *CFE_ES_STOP_APP* and *GENERIC_ADCS_SET_MODE_CC* (end of the SPP packet). One can use the same approach to define other critical TCs.
5. Watchdog messages are exchanged among the satellites via the ISL (as TM messages). 
6. CFDP layer is based on external python library [here](https://gitlab.com/librecube/lib/python-cfdp). See *CF_UPLINK_FILE* / *CF_DOWNLINK_FILE* commands.
7. The ground stations used for visibility computation are in *Inp_Sim.txt*. The simulator consider a simple dynamic routing algorithm by default (simple ring topology with 1 feeder link). 

**Development**
1. The simulator development can be managed completely from SAT1 VM. We suggest to modify only SAT1 and then to diffuse the modifications. This is managed via bash scripts in *Install_tools_nos3* folder.
 - *./Do_Update_All.sh* to diffuse the modifications 
 - *./Do_Make_All_VM.sh* to recompile
 - Only for modifications in COSMOS files, compile (as root) also on MCS VM (call *make* in github-nos3 folder).
2. You can clean using *./Do_MakeClean_All_VM.sh* plus *./Do_Make_All_VM.sh* to recompile.


<!-- PUBLICATIONS EXAMPLES -->
## Publications

* [ESA 3S 2025](https://indico.esa.int/event/571/attachments/7211/13615/A%20Satellite%20Constellation%20Simulator%20for%20Space.pdf): A satellite constellation simulator for space systems cybersecurity research and development. (Introduction)
* [IAC 2025](https://dl.iafastro.directory/event/IAC-2025/paper/96166/): A novel simulation platform for space systems cybersecurity R&D. (Focused on attacks and mitigations)
* [EDCC 2026](https://www.cs.kent.ac.uk/EDCC2026/home): Attack Scenarios and Embedded Intrusion Detection for Space Systems.
* [WORK IN PROGRESS]()

If you find this work useful, please acknowledge it by citing these papers.

A simple guide for this simulator can be found in [WORK IN PROGRESS]() 

<!-- LICENSE -->
## License
NOS3 is distributed under the NOSA 1.3 License that can be found in github-nos3 folder. 
The CSS simulator is also distributed under the NOSA 1.3 License. See `LICENSE.txt` for more information. 

<!-- DISCLAIMER -->
## DISCLAIMER
The provided content is for research and testing. We are not responsible for any inappropriate usage of this content. 
The code and the main VM are provided as-is for general use. We do not offer dedicated support or troubleshooting assistance.

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [NASA](https://www.nasa.gov/jon-mcbride-software-testing-and-research-jstar/) for sharing NOS3 simulator with the community.
* [IRT](https://www.irt-saintexupery.com/) for driving and supporting the project.
* [STARION](https://www.stariongroup.eu/) and [NEXOVA](https://www.nexovagroup.eu/) for driving the development and providing access to CITEF platform. 
* [Thales Alenia Space](https://www.thalesaleniaspace.com/fr) for driving and supporting the development.
* [LAAS-CNRS](https://www.laas.fr/en/) for the scientific and technical contribution.
* [Gatewatcher](https://www.gatewatcher.com/) for the support and access to ground probes.
* [French Air Force Academy](https://www.ecole-air-espace.fr/) for the institutional and scientific support.
* [CNES](https://cnes.fr/) for the institutional and scientific support.



</div>
