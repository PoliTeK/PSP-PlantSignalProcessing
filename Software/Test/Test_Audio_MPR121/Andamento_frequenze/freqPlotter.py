import matplotlib.pyplot as plt
import csv
import numpy as np
import argparse
import scipy.signal as flt

# parser initialization
parser = argparse.ArgumentParser()

# positional argument 
parser.add_argument ("input_filename", help='input filename where data is stored')
args = parser.parse_args()

# open txt file
rawValues = [0] 
with open (args.input_filename, mode ='r') as file:
    csvFile = csv.reader(file)
    
    # divide elements of every row in different arrays
    for lines in csvFile:    
        rawValues.append(lines[1])

# filtering 
numtaps = 3
cutoff = 0.1
coefficients = flt.firwin (numtaps,cutoff)
filtered = flt.lfilter(coefficients, 1.0 , stof (rawValues)) 
print (filtered)

x = np.linspace (0,len(rawValues),len(rawValues))

# plotting section

plt.plot (x, rawValues[:])
plt.ylabel ("Value")
plt.xlabel ("Sample Number")
plt.show()
