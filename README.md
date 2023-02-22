# PID_motor_controller
PID control with DC motor using NEXYS A7 (Xilinx A7 FPGA)

# Version 0.1 - Testing Xilinx Versioning 

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
    - rebuild.tcl        - rebuilds vivado project
    - hdl/               - project hdl code 
    - xdc/               - constraints 
    - motor_hardware/    - vivado project (not checked in)
    - IP/                - custom IP 
- PID_motor_controller_vitis (TBD)
    - PID_motor_controller_vitis_app
    - PID_motor_controller_vitis_app_system
    - PID_motor_controller_vitis_platform 