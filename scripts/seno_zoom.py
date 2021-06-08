import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 200

fm, data_seno = wavfile.read('doremi_seno.wav')
fm, data_seno_without = wavfile.read('doremi_seno_without_interp.wav')

first_index = 20500
last_index = 20600

total_length_samples = len(data_seno)
total_length_s = total_length_samples/fm
n_axis = np.linspace(0, total_length_s, total_length_samples)

plt.step(n_axis[first_index:last_index],data_seno[first_index:last_index], c='tab:red',linewidth = 0.6,label='Seno with interpolation')
plt.step(n_axis[first_index:last_index],data_seno_without[first_index:last_index], c='tab:blue',linewidth = 0.6,label='Seno without interpolation')

plt.title("Seno zoomed in waveform")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")

leg = plt.legend()
for legobj in leg.legendHandles:
    legobj.set_linewidth(1.0)