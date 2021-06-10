import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 200

fm, data_seno = wavfile.read('distortion.wav')
fm, data_seno_without = wavfile.read('distortion_noclip.wav')
fm, data_seno_soft = wavfile.read('distortion_soft_clip.wav')

first_index = 300
last_index = 800

total_length_samples = len(data_seno)
total_length_s = total_length_samples/fm
n_axis = np.linspace(0, total_length_s, total_length_samples)

plt.step(n_axis[first_index:last_index],data_seno_without[first_index:last_index], c='tab:blue',linewidth = 0.6,label='Percussion without clipping')
plt.step(n_axis[first_index:last_index],data_seno_soft[first_index:last_index], c='tab:orange',linewidth = 0.6,label='Percussion with soft-clipping')
plt.step(n_axis[first_index:last_index],data_seno[first_index:last_index], c='tab:red',linewidth = 0.6,label='Percussion with clipping')

plt.title("Distorted waveform")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")

leg = plt.legend(loc='upper right', prop={'size': 8})
for legobj in leg.legendHandles:
    legobj.set_linewidth(1.5)