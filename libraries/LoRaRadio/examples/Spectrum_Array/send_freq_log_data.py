import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import csv
import numpy as np
from threading import Thread
import time 
from datetime import datetime

#Serial stuff
ser = serial.Serial('COM3', 500000, timeout=1)
ser.reset_input_buffer()

def read_serial():
    '''
    Simple serial read function
    '''
    received = ser.readline()
    return received.decode("utf-8").rstrip()


def write_serial(string):
    '''
    Simple serial write function
    '''
    string += "\n"
    ser.write(string.encode())


def read_data():
    state = "wait print"
    frequencies = []
    rssi_values = []
    count = 0
    print("ENTERED")
    while True:
        decoded = read_serial()
        print(decoded)

        if state == "wait print":
            if decoded == "PRINT_TOKEN":
                state = "read values"
                #print("STATE: " + state)
            elif decoded == "READY_TOKEN":
                count += 1
                if count > 5:
                    # Something went wrong
                    return 0, 0


        elif state == "read values":
            if decoded == "DONE_TOKEN":
                print("RETURNED")
                return frequencies, rssi_values 
            elif decoded == "READY_TOKEN":
                # Something went wrong
                return 0, 0
            else:
                try:
                    split_line = decoded.split(",")
                    frequencies.append(float(split_line[0]))
                    rssi_values.append(int(split_line[1]))
                except Exception as e: # work on python 3.x
                    print('Something went wrong: '+ str(e))
                    return 0, 0


fig = plt.figure()
ax = plt.axes(ylim=(-200, 0))
#line, = ax.stem(1, use_line_collection=True)
fig_num = 0 

f = open("freq_log.txt", "w")

while True:

    while True:
        decoded = read_serial()
        if decoded == "READY_TOKEN":
            break

    ser.reset_input_buffer()
    write_serial("START_TOKEN")
    #time.sleep(0.01); 
    frequencies, rssi_values = read_data()
    if frequencies == 0:
        continue

    ax.clear()
    markerline, stemline, baseline, = ax.stem(frequencies, rssi_values, bottom=-200, use_line_collection=True)

    ax.set_ylim(-200, 0)
    plt.setp(stemline, linewidth = 0.25)
    plt.setp(markerline, markersize = 1)

    time=datetime.now(tz=None)
    f.write(str(time))
    f.write("\n")
    f.write(str(frequencies))
    f.write("\n")
    f.write(str(rssi_values))
    f.write("\n\n\n")
    #f.close()

    avg_power = np.average(rssi_values) 
    ax.set_title("Average power of specter:  {}".format(avg_power))
    
    if avg_power > -70.5:
        plt.savefig('specter{}.png'.format(fig_num))
        fig_num += 1
        plt.pause(1)
    else:
        plt.pause(0.03)
