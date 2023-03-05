'''
    @file plot display.py - program to read PID control loop data printed to 
    console via uartlite on nexys a7

    @authors Stephen, Drew, Noah 
    @copyright Portland State University, 2023

    @brief this python script plots data printed to console via uartlite

'''

# Reference to: http://www.mikeburdis.com/wp/notes/plotting-serial-port-data-using-python-and-matplotlib/

import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
import numpy as np 
import serial 
import sys
import csv 


#serial port
uartlite = serial.Serial()
uartlite.port = ''
uartlite.baudrate = 9600
uartlite.timeout  = 10

#csv file 
filename = 'PID_data.csv'



#global lists 
time = []
Kp = []
Ki = []
Kd = []
Set = []
Read = []
Error = []

data_display = plt.figure(figsize=(10,5))
ax = data_display.add_subplot(1,1,1)

def updateData(i):
    data = uartlite.readline()
    parsed_data = data.split(b' ')
    #check for control signal
    if (parsed_data[0] != b'DB'):
        print(data.decode(), end="")
        return 
    if (i == 1000):
        file.close()
        exit() 

    curr_set = int(parsed_data[1]); 
    curr_read = int(parsed_data[2]); 
    curr_error = int(parsed_data[1]) - int(parsed_data[2])
    curr_Kp = int(parsed_data[3]); 
    curr_Ki = int(parsed_data[4]); 
    curr_Kd = int(parsed_data[5]); 

    if (i > 0): 
        row = [i, curr_set,curr_read, 
               curr_error, curr_Kp, 
                curr_Ki, curr_Kd]
        writer.writerow(row)
  

    time.append(i)
    Set.append(curr_set)
    Read.append(curr_read)
    Error.append(curr_error)
    Kp.append(curr_Kp)
    Ki.append(curr_Ki)
    Kd.append(curr_Kd)

    ax.clear()
    ax.plot(time, Set, label=f'set rpm = {curr_set}')
    ax.plot(time, Read, label=f'read rpm = {curr_read}')
    ax.plot(time, Error, label=f'error = {curr_error}')
    ax.plot(time, Kp, label=f'Kp = {curr_Kp}')
    ax.plot(time, Ki, label=f'Ki = {curr_Ki}')
    ax.plot(time, Kd, label=f'Kd = {curr_Kd}')
    
    plt.xlim([0, max(100,i)])
    plt.ylim([-20,100])
    plt.title("Paramaters over Time")
    plt.xlabel("Time in Seconds")
    plt.ylabel("Paramaters")
    plt.locator_params(axis='x', nbins=20)
    plt.locator_params(axis='y', nbins=20)
    plt.legend()
  


def parseArgs(argv, filename):
    status = False 
    for i in range(1, len(sys.argv)):
        if (sys.argv[i] == '-port'):
            uartlite.port = sys.argv[i+1]
            status = True
        if (sys.argv[i] == '-outfile'): 
            filename = sys.argv[i+1]
    return status, filename


if __name__ == "__main__":
    [status, filename] = parseArgs(sys.argv, filename)
    if (status != True):
        print("error with port or outfile")
        print("please supply a valid port")
        print("if specifying an outfile with -outfile")
        print("please specify filename")
        exit()
    uartlite.open()
    file = open(filename, 'w')
    writer = csv.writer(file)
    header = ['time', 'set rpm', 'read rpm', 'error', 'Kp', 'Ki', 'Kd']
    writer.writerow(header)
    graph = animation.FuncAnimation(data_display, updateData, interval=100, save_count=1000)
    plt.show()
