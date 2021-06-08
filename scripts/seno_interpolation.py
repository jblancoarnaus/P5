import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 200

data_in = np.loadtxt('seno.txt')
generated_signal = data_in[:,0]
table_floor = data_in[:,1]
table_ceil = data_in[:,2]

first_index = 8540
last_index = 8600
fm = 44100;

total_length_samples = len(generated_signal)
total_length_s = total_length_samples/fm
n_axis = np.linspace(0, total_length_s, total_length_samples)

plt.step(n_axis[first_index:last_index],generated_signal[first_index:last_index],'o',markersize = 2, c='tab:red',label='Interpolated value')
plt.step(n_axis[first_index:last_index],table_floor[first_index:last_index], 'o',markersize = 2,c='tab:blue',label='Table value (floor)')
plt.step(n_axis[first_index:last_index],table_ceil[first_index:last_index], 'o',markersize = 2,c='tab:green',label='Table value (ceil)')

plt.title("Generated signal vs table values")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")

leg = plt.legend()
for legobj in leg.legendHandles:
    legobj.set_linewidth(2.0)