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
file = open(filename, 'w')
writer = csv.writer(file)
header = ['time', 'set rpm', 'read rpm', 'error', 'Kp', 'Ki', 'Kd']
writer.writerow(header)


#global lists 
time = []
Kp = []
Ki = []
Kd = []
Set = []
Read = []
Error = []

data_display = plt.figure()
ax = data_display.add_subplot(1,1,1)

def updateData(i):
    data = uartlite.readline()
    parsed_data = data.split(b' ')
    #check for control signal
    if (parsed_data[0] != b'DB'):
        return 
    if (i == 1000):
        file.close()
        exit() 
    if (i > 0): 
        row = [i, int(parsed_data[1]),int(parsed_data[2]), 
               int(parsed_data[1]) - int(parsed_data[2]), (int(parsed_data[3])), 
                (int(parsed_data[4])), (int(parsed_data[5]))]
        writer.writerow(row)

    time.append(i)
    Set.append(int(parsed_data[1]))
    Read.append(int(parsed_data[2]))
    Error.append(int(parsed_data[1]) - int(parsed_data[2]))
    Kp.append((int(parsed_data[3])))
    Ki.append((int(parsed_data[4])))
    Kd.append((int(parsed_data[5])))

    ax.clear()
    ax.plot(time, Set, label="set")
    ax.plot(time, Read, label="read rpm")
    ax.plot(time, Error, label="error")
    ax.plot(time, Kp, label="Kp")
    ax.plot(time, Ki, label="Ki")
    ax.plot(time, Kd, label="Kd")
    
    plt.xlim([0, max(100,i)])
    plt.ylim([0,100])
    plt.title("Paramaters over Time")
    plt.xlabel("Time in Seconds")
    plt.ylabel("Paramaters")
    plt.legend()
  


def parseArgs(argv):
    status = False 
    for i in range(1, len(sys.argv)):
        if (sys.argv[i] == '-port'):
            uartlite.port = sys.argv[i+1]
            status = True
        if (sys.argv[i] == '-outfile'): 
            filename = sys.argv[i+1]
    return status


if __name__ == "__main__":
    status = parseArgs(sys.argv)
    if (status != True):
        print("error with port or outfile")
        print("please supply a valid port")
        print("if specifying an outfile with -outfile")
        print("please specify filename")
        exit()
    uartlite.open()
    graph = animation.FuncAnimation(data_display, updateData, interval=200, save_count=1000)
    plt.show()
