# Version 1.0 - PID Motor Controller 

Drew Seidel (dseidel@pdx.edu)\
Stephen Weeks (stweeks@pdx.edu)\
Noah Page (nopage@pdx.edu)

PID control with DC motor using NEXYS A7 (Xilinx A7 FPGA)

# Repository Organization 
- bitstream 
    - contains bitstream with PID controller app elf associated. Download this bitstream to a flash drive and connect it to the Nexys A7 with USB mode selected on JP1 and JP2 to run program
- docs
    - project design report 
    - contains flow_charts subdirectory for project flow charts
    - a serial monitoring guide 
    - the project embedded system hardware pdf
- PID_motor_controller_vivado
    - motor_hardware.tcl        - rebuilds vivado project
    - hdl               - project hdl code 
    - xdc               - constraints 
    - motor_hardware    - vivado project (not checked in)
    - IP                - project IP 
        - ece544ip_w23     - (contains and adds to the entire ece544ip_w23 directory for the tcl scripting and revisioning purposes. The custom IP for this project, **myHB3ip_1.0**, can be found here
        - bd   - contains embedded system board desing (needed for tcl script to build project)
       - Pmod_Bridge_v1_0 - included in project release, not utilized in our project
- logger
    - csv - contains cvs files from prior tests 
    - photos - contains graphs from prior tests
    - plot_display.py - live python plotter and csv writer 
    - README.md (see read me for live plotter usage instructions)
- src 
    - contains all C source files for the application (project developed using the Xiling Vitis Software Platform)

# Instructions for Building Project in Vivado 

This repository assumes that you are running Vivado 2019.2

``` sh
git clone https://github.com/drew-griffin/PID_motor_controller.git
```
``` sh
cd PID_motor_controller_vivado
```
``` sh
rm -rf motor_hardware #run after first time
```
``` sh 
vivado -source rebuild.tcl
```

# Instructions for Running Live Plotter 
See README in logger directory for a detailed explanation
