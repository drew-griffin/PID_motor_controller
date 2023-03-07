'''
    @file plot display.py - program to read PID control loop data printed to 
    console via uartlite on nexys a7

    @authors Stephen, Drew, Noah 
    @copyright Portland State University, 2023

    @brief this python script plots data printed to console via uartlite
           as well as writes to a CSV file 
    @arguments -port <specify port for logging> REQUIRED
               -outfile <specific CSV output file> if none given default to PID_data.csv

    Reference to: http://www.mikeburdis.com/wp/notes/plotting-serial-port-data-using-python-and-matplotlib/               
'''



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

#figure declare
data_display = plt.figure(figsize=(10,5))
ax = data_display.add_subplot(1,1,1)

#update data called by the animator 
#frame interval = 100ms 
#i is frame number argument passed in by default 
#because data is read every second 
#by uartlite specification, add i will 
#represent seconds  
def updateData(i):
    data = uartlite.readline()
    parsed_data = data.split(b' ')
    #check for control signal
    if (parsed_data[0] != b'DB'):
        #if not the control signal, print the uartlite 
        #message
        print(data.decode(), end="")
        return 
    #if i is 1000, end the program. 
    if (i == 1000):
        file.close()
        exit() 

    #data format: DB <set rpm> <read rpm> <Kp> <Ki> <Kd>
    #error calculated 
    #find current metrics for everything 
    curr_set = int(parsed_data[1]); 
    curr_read = int(parsed_data[2]); 
    curr_error = int(parsed_data[1]) - int(parsed_data[2])
    curr_Kp = int(parsed_data[3]); 
    curr_Ki = int(parsed_data[4]); 
    curr_Kd = int(parsed_data[5]); 
    
    #create new CSV row with current data, and write to csv
    if (i > 0): 
        row = [i, curr_set,curr_read, 
               curr_error, curr_Kp, 
                curr_Ki, curr_Kd]
        writer.writerow(row)
  
    #append global lists with the new current value
    time.append(i)
    Set.append(curr_set)
    Read.append(curr_read)
    Error.append(curr_error)
    Kp.append(curr_Kp)
    Ki.append(curr_Ki)
    Kd.append(curr_Kd)

    #plot new lists with appending (helped by animator)
    ax.clear()
    ax.plot(time, Set, label=f'set rpm = {curr_set}')
    ax.plot(time, Read, label=f'read rpm = {curr_read}')
    ax.plot(time, Error, label=f'error = {curr_error}')
    ax.plot(time, Kp, label=f'Kp = {curr_Kp}')
    ax.plot(time, Ki, label=f'Ki = {curr_Ki}')
    ax.plot(time, Kd, label=f'Kd = {curr_Kd}')
    
    #allow movable x scale 
    plt.xlim([0, max(100,i)])
    plt.ylim([-20,100])
    plt.title("Paramaters over Time")
    plt.xlabel("Time in Seconds")
    plt.ylabel("Paramaters")
    plt.locator_params(axis='x', nbins=20)
    plt.locator_params(axis='y', nbins=20)
    plt.legend()
  

#parseArge parses the arguments supplied on command line
#returns true if uartlite is specified (required)
def parseArgs(argv, filename):
    status = False 
    for i in range(1, len(sys.argv)):
        if (sys.argv[i] == '-port'):
            uartlite.port = sys.argv[i+1]
            status = True
        if (sys.argv[i] == '-outfile'): 
            filename = sys.argv[i+1]
    return status, filename

#main program. 
#parse args first, then check status. 
#if true, proceed, else, end program
if __name__ == "__main__":
    [status, filename] = parseArgs(sys.argv, filename)
    if (status != True):
        print("error with port or outfile")
        print("please supply a valid port")
        print("if specifying an outfile with -outfile")
        print("please specify filename")
        exit()
    uartlite.open() #open the uartlite
    file = open(filename, 'w') #open the specified program 
    writer = csv.writer(file) #specifiy the csv ßwrite
    header = ['time', 'set rpm', 'read rpm', 'error', 'Kp', 'Ki', 'Kd']
    writer.writerow(header) #write the header row for the csv file

    #setup the animator function. data_display is the figure, updateData is the function called, 
    #frame interval is 100ms (must be faster than the sample rate, in this case 1second), 
    #save count is the number of frames to cache
    graph = animation.FuncAnimation(data_display, updateData, interval=100, save_count=1000)
    plt.show()
