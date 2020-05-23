import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import csv
import numpy as np
from threading import Thread
import time 

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
    state = "read values"
    frequencies = []
    rssi_values = []

    while True:
        decoded = read_serial()
        print(decoded)

        """
        if state == "start":
            if decoded == "Spectrum done":
                state = "read values"
                #print("STATE: " + state)
        """
        if state == "read values":
            if decoded == "READY_TOKEN":
                #Something went wrong
                return 0, 0, 0, 0
            try:
                split_line = decoded.split(",")
                if len(split_line) == 3:
                    start = float(split_line[0])
                    end = float(split_line[1])
                elif len(split_line) == 2:
                    frequencies.append(float(split_line[0]))
                    rssi_values.append(int(split_line[1]))
                else:
                    return frequencies, rssi_values, start, end
            except Exception as e: # work on python 3.x
                    print('Something went wrong: '+ str(e))
                    return 0, 0, 0, 0


fig = plt.figure()
ax = plt.axes(ylim=(-200, 0))
line, = ax.plot([], [])
fig_num = 0 
while True:

    while True:
        decoded = read_serial()
        if decoded == "READY_TOKEN":
            break

    write_serial("START_TOKEN")
    time.sleep(0.01); 
    frequencies, rssi_values, start, end= read_data()
    ax.clear()
    ax.plot(frequencies, rssi_values)
    ax.set_xlim(start, end)
    ax.set_ylim(-200, 0)

    avg_power = np.average(rssi_values) 
    ax.set_title("Average power of specter:  {}".format(avg_power))
    
    if frequencies == 0:
        continue

    if avg_power > -104.5:
        plt.savefig('specter{}.png'.format(fig_num))
        fig_num += 1
        plt.pause(1)
    else:
        plt.pause(0.03)
