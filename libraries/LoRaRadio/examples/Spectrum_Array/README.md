# Spectrum scan of desired frequencies

This example contains everything that is needed to scan desired frequency range, log results and plot them as waterfall.

## Prerequisites 

* Any development board with CMWX1ZZABZ-078 module. Common example of dev board with this chip is B-L072Z-LRWAN1 development board from ST.
* Chip needs to be flashed with `Spectrum_Array.ino` sketch.
* Python 3.6 or higher with modules matplotlib, numpy and argparse.

## Frequency file 
1. Frequencies that we wish to scan need be written in one line, separated by comma ( , ) and save into a txt file.
2. Frequencies should be in MHz, for example: 880.2, 880.4, 880.6, 900.5, 895.
3. Frequencies will be scanned in order they are listed.


## Usage
1. Create txt file that holds a list of frequencies that you want to scan.
2. Connect dev board to PC.
3. Run `send_freq_log_data.py` with arguments about port name and path to frequencies text file.
4. Script will send frequencies to dev board, which will start the scan, this is indicated by blue blinking led.
5. During scan user can see stem plot of rssi values, as well as frequency, rssi value pairs in terminal output.
6. Scan can be stopped anytime, either by unplugging the board or stopping the script.
7. Logged data is stored in `freq_log.txt` file.
8. Logged data can be displayed in waterfall plot by calling `plot_data.py` script.
9. Dev board should be reset before attempting to scan again.
