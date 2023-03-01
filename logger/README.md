# Install python dependencies 
 
 - Numpy 
 - Matplotlib
 - serial 
 - csv
 - sys

 # To run find find the port that the Nexys A7 is plugged into. For example: 
 ```sh
 python3 plot_display.py -port /dev/tty.usbserial-FPGA
 ```
# To run while specifying filename: 
 ```sh
 python3 plot_display.py -port /dev/tty.usbserial-FPGA -outfile testPID.csv
 ```
# About
This script runs a live grapher of the data received over uart from the FPGA (max time duration is 1000 seconds). It also generates a csv file of the corresponding data. 