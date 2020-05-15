import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import csv
import numpy as np
from threading import Thread

#Serial stuff
ser = serial.Serial('/dev/ttyACM0', 115200)
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
        #print(decoded)

        if state == "start":
            if decoded == "Spectrum done":
                state = "read values"
                #print("STATE: " + state)

        elif state == "read values":
            split_line = decoded.split(",")
            if len(split_line) == 2:
                frequencies.append(float(split_line[0]))
                rssi_values.append(int(split_line[1]))
            else:
                return frequencies, rssi_values


fig = plt.figure()
ax = plt.axes(xlim=(868, 900), ylim=(-200, 0))
line, = ax.plot([], [])

while True:

    write_serial("START_TOKEN")
    frequencies, rssi_values = read_data()
    line.set_data(frequencies, rssi_values)

    plt.pause(0.1)
