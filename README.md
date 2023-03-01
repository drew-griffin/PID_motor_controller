# PID_motor_controller
PID control with DC motor using NEXYS A7 (Xilinx A7 FPGA)

# Version 0.1 - Testing Xilinx Versioning 
- This repository assumes that the ece544ip_w23 IP has already been added to your Vivado. 
- This repository assumes that you are running Vivado 2019.2

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

# Repository Organization 
- PID_motor_controller_vivado
    - motor_hardware.tcl        - rebuilds vivado project
    - hdl/               - project hdl code 
    - xdc/               - constraints 
    - motor_hardware/    - vivado project (not checked in)
    - IP/                - custom IP 
