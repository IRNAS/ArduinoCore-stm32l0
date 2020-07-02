# Scientific and vector computation for python
import numpy as np

# Plotting library
import matplotlib.pyplot as plt
import matplotlib.dates as md
import matplotlib.colors as colors

import sys
import argparse
from argparse import RawTextHelpFormatter

def read_data(filename):
    """Reads frequency data from given text file"""

    num_lines = sum(1 for line in open(filename))

    timestamp = []
    freq = []
    rssi_values = []
    # Read
    with open(filename, "r") as f:
        for i in range(int(num_lines/5)):
            timestamp.append(f.readline())
            freq.append(f.readline().replace("[", "").replace("]", "").split(","))
            rssi_values.append(f.readline().replace("[", "").replace("]", "").split(","))
            dummy = f.readline()
            dummy = f.readline()

    # Convert into right types
    freq = np.array(freq)
    rssi_values = np.array(rssi_values)
    freq = freq.astype(np.float)
    rssi_values = rssi_values.astype(np.int)
    #print(freq[0])
    #print(type(rssi_values[0][0]))

    # Sort
    sorted_indexes = np.argsort(freq[0])
    for i in range(freq.shape[0]):
        freq[i] = freq[i][sorted_indexes]

    for i in range(freq.shape[0]):
        rssi_values[i] = rssi_values[i][sorted_indexes]


    # Create extended freq range 
    extended_freq = np.arange(np.min(freq), np.max(freq)+0.2, 0.2)
    extended_freq = np.round(extended_freq, 1)


    # Create extended rssi values, empty ones are filled with -130 
    extended_rssi = np.zeros((freq.shape[0], len(extended_freq)))
    #print(extended_rssi.shape)
    #print(rssi_values.shape)

    for i in range(extended_rssi.shape[0]):
        for j in range(extended_rssi.shape[1]):
            if extended_freq[j] in freq[i]:
                idx = np.where(freq[i] == extended_freq[j])
                extended_rssi[i][j] = rssi_values[i][idx]
            else:
                extended_rssi[i][j] = -130

    return freq, extended_rssi


def plot_waterfall(filename, frequencies, rssi):
    """Plots waterfall graph"""

    # Flip y axis twice, first in linspace, then with invert_yaxis()
    x, y = np.meshgrid(np.arange(np.min(frequencies), np.max(frequencies)+0.2, 0.2), 
          np.linspace(frequencies.shape[0], 0, frequencies.shape[0], endpoint=False))

    # Create pcolor plot, use all rows, skip first column which contains frequencies
    fig, ax = plt.subplots(figsize=(20, 10))
    c = ax.pcolor(x, y, rssi, vmin=-120, vmax=-70)

    # Added title limits and colorbar
    plt.gca().invert_yaxis()
    ax.set_title(filename)
    ax.set_xlabel('Frequency [MHz]')
    ax.set_ylabel('Time[s]')
    #ax.set_ylim([ymin,ymax])
    fig.colorbar(c, ax=ax)
    plt.savefig(filename.split(".")[0] + ".png", bbox_inches='tight')
    plt.show()


def simple_plot(frequencies, rssi, frequency):
    """
    Filters specified frequency from data and plots its rssi into a simple plot.
    """
    
    # Find index of specified frequency, if not found then exit program
    freq_index = np.where(frequencies[0] == float(frequency))
    if (freq_index[0].size == 0):
        print("Frequency was not found, exiting")
        sys.exit(1)

    # Something weird happend while getting index, it got returned as a 2d
    # array, so i need to acces this value
    freq_index = freq_index[0][0]
    print(freq_index)
    # Filter out rssi values with that index 
    filtered_rssi = rssi[:, freq_index]

    print(rssi.shape)
    print(filtered_rssi.shape)


    # Plot, use specified frequency as title
    fig, ax = plt.subplots(figsize=(20, 10))
    ax.plot(filtered_rssi[::-1]) # We need to switch order
    ax.set_title(frequency + " MHz")
    ax.set_xlabel('Time[s]')
    ax.set_ylabel('Rssi')
    #ax.set_ylim([ymin,ymax])
    plt.savefig(frequency + "MHz.png", bbox_inches='tight')
    plt.show()




### Command line argument parser

# Intro message
intro = "This is a spectrum ploter!\n\n"
intro += "Input data logged in txt file and you will receive spectrum image in png format."
parser = argparse.ArgumentParser(description=intro, formatter_class=RawTextHelpFormatter)
parser.add_argument("--file", type=str,required=True, help="Path to file")
parser.add_argument("--one_freq", type=str,required=False, help="If you want to extract one frequency from the full freq-log.txt and plot it, specify it in MHz")

# Display help message if no arguments are provided
if len(sys.argv)== 1:
    parser.print_help(sys.stderr)
    sys.exit(1)
args = parser.parse_args()

# Read data from file
frequencies, rssi = read_data(args.file)
if args.one_freq is None:
    # Optional argument not given, lets just plot waterfall
    plot_waterfall(args.file, frequencies, rssi)
else:
    # Single frequency argument was defined, lets use it
    simple_plot(frequencies, rssi, args.one_freq)
