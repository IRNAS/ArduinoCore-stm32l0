import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import csv
import numpy as np
from threading import Thread

#Serial stuff
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=3)
ser.flushInput()

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


import matplotlib.pyplot as plt
import numpy as np

def read_data():
    state = "start"
    frequencies = []
    rssi_values = []

    while True:
        decoded = read_serial()
        print(decoded)

        if state == "start":
            if decoded == "Spectrum done":
                state = "read values"
                #print("STATE: " + state)

        elif state == "read values":
            try:
                split_line = decoded.split(",")
                if len(split_line) == 2:
                    frequencies.append(float(split_line[0]))
                    rssi_values.append(int(split_line[1]))
                else:
                    return frequencies, rssi_values
            except Exception as e: # work on python 3.x
                    print('Something went wrong: '+ str(e))
                    return 0, 0


fig = plt.figure()
ax = plt.axes(xlim=(883, 888), ylim=(-200, 0))
line, = ax.plot([], [])
fig_num = 0 
while True:

    while True:
        decoded = read_serial()
        if decoded == "READY_TOKEN":
            break

    write_serial("START_TOKEN")
    frequencies, rssi_values = read_data()
    ax.clear()
    ax.plot(frequencies, rssi_values)
    ax.set_xlim(883, 888)
    ax.set_ylim(-200, 0)
    avg_power = np.average(rssi_values) 
    ax.set_title("Average power of specter:  {}".format(avg_power))
    
    if frequencies == 0:
        continue

    if avg_power > -105:
        plt.savefig('specter{}.png'.format(fig_num))
        fig_num += 1
        plt.pause(2)


    plt.pause(0.1)
