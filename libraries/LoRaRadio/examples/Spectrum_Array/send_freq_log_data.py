import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import serial
import time
import csv
from threading import Thread
from datetime import datetime
import sys
import argparse
from argparse import RawTextHelpFormatter

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


## Start of program
### Command line argument parser

# Intro message
intro = "This is frequency uploader and data logging tool!\n\n"
intro += "This tool expects in its folder the 'frequencies.txt' text file, where frequencies \nthat we wish to scan are all written in one line and separated by comma ( , ). \nFrequencies should be in MHz, for example: 880.2, 880.4, 880.6, 900.5, 895\nIn order frequencies are listed, in that order they will be scanned."

parser = argparse.ArgumentParser(description=intro, formatter_class=RawTextHelpFormatter)
parser.add_argument("--port", type=str,required=True, help="Serial port that we should listen to, something like COM3, or /dev/ttyACM0")

# Display help message if no arguments are provided
if len(sys.argv)== 1:
    parser.print_help(sys.stderr)
    sys.exit(1)
args = parser.parse_args()

with open("frequencies.txt", "r") as f:
    freq = f.readline()

# Frequencies is a list of strings
frequencies = freq.split(",")
frequencies = [float(i) for i in frequencies]
frequencies = [str(i) for i in frequencies]

# Serial stuff
ser = serial.Serial(args.port, 115200, timeout=1)
ser.flushInput()
ser.flushOutput()

# Prepare plot
fig = plt.figure()
ax = plt.axes(ylim=(-150, -30))
fig_num = 0 

# Send frequencies to device
while True:
    decoded = read_serial()
    if decoded == "READY_TOKEN":
        break

ser.reset_input_buffer()

print("Started sending frequency array")
for i in range(len(frequencies)):
    write_serial(frequencies[i])
    while True:
        decoded = read_serial()
        if decoded == "OK":
            break
        elif decoded == "NOT OK":
            i -= 1
            break


print("Frequency array sent, logging started")
ser.reset_input_buffer()
write_serial("START_TOKEN")

f = open("freq_log.txt", "w")
while True:

    while True:
        decoded = read_serial()
        if decoded == "READY_TOKEN":
            break

    ser.reset_input_buffer()
    write_serial("START_TOKEN")
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

    avg_power = np.average(rssi_values) 
    ax.set_title("Average power of specter:  {}".format(avg_power))
    
    plt.pause(0.03)
