import numpy as np
import math
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 300

fm, data_in = wavfile.read('doremi.wav')
#assign expected length (s) of each part so labels are assigned properly
ADSR_A = 0.02
ADSR_D = 0.1
ADSR_R = 0.1

# a priori we don't know how many samples the S state lasts -> find first silence (at least two consecutive zero crossings)
for i in range(len(data_in)-1):
  if data_in[i]==0 and data_in[i+1]==0:
    total_length_samples = i
    break
total_length_s = total_length_samples/fm

#convert to samples
n_A = math.floor(ADSR_A*fm)
n_D = math.floor(ADSR_D*fm)
n_R = math.floor(ADSR_R*fm)

#sample axis in seconds
n_axis = np.linspace(0, total_length_s, total_length_samples)

plt.plot(n_axis[0:n_A],data_in[0:n_A], 'tab:red',linewidth=0.15,label='A')
plt.plot(n_axis[n_A:n_A+n_D],data_in[n_A:n_A+n_D], 'tab:orange',linewidth=0.15,label='D')
plt.plot(n_axis[n_D+n_A:total_length_samples-n_R],data_in[n_D+n_A:total_length_samples-n_R], 'tab:olive',linewidth=0.15,label='S')
plt.plot(n_axis[total_length_samples-n_R:total_length_samples],data_in[total_length_samples-n_R:total_length_samples], 'tab:green',linewidth=0.15,label='R')

plt.title("DumbInstrument waveform")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")

leg = plt.legend()
for legobj in leg.legendHandles:
    legobj.set_linewidth(2.0)
