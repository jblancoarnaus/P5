import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 200
import scipy.interpolate as spi

fm, data = wavfile.read('seno_tremolo.wav')
first_index_s = 0

x = np.array(data)

# trim first note
# a priori we don't know how many samples the S state lasts -> find first silence (at least two consecutive zero crossings)
for i in range(len(x)-1):
  if x[i]==0 and x[i+1]==0:
    total_length_samples = i
    break

first_index=0
last_index = total_length_samples

x = x[first_index:last_index]

pos = np.diff(np.sign(np.diff(x))).nonzero()[0] + 1 # find local peaks and valleys position

peaks=[]
valleys=[]
pos_peaks=[]
pos_valleys=[]
peaks_valleys=[]
for i in range(len(pos)):
  value = x[pos[i]]
  index = pos[i]

  peaks_valleys.append(value)
  if value > 0:
    peaks.append(value)
    pos_peaks.append(index)

  else:
    valleys.append(value)
    pos_valleys.append(index)

#convert to seconds
pos_peaks=np.array(pos_peaks)/fm
pos_valleys=np.array(pos_valleys)/fm
pos=pos/fm

#interpolate values
f_peaks = spi.interp1d(pos_peaks,peaks, kind='cubic',bounds_error=False)
f_valleys = spi.interp1d(pos_valleys,valleys, kind='cubic',bounds_error=False)

t = np.linspace(first_index_s,last_index/fm,num=len(x))
interpolated_valleys = f_valleys(t)
interpolated_peaks = f_peaks(t)

state = 0
state_prev = 0
maxfound=0
for i in range(len(peaks)-1):
  if (peaks[i+1]-peaks[i]) > 0:
    state = 1
  else:
    state = -1
  if state_prev == 1 and state == -1:
    max_pos = np.where(x == peaks[i]) #!! unreliable !! might not work if there's a sample with the same value before it
    max_pos = max_pos[0]
    max_pos = max_pos[0]
    maxfound=1

  if maxfound==1 and state_prev == -1 and state == 1 :
      min_pos = np.where(x == peaks[i])
      min_pos = min_pos[0]
      min_pos = min_pos[0]
      break
  state_prev = state

first_max = x[max_pos]
first_min = x[min_pos]

plt.plot(t, x, c='tab:blue',linewidth = 0.4,label='Signal')
plt.plot(t[max_pos:max_pos+(min_pos-max_pos)*2], x[max_pos:max_pos+(min_pos-max_pos)*2], c='tab:orange',linewidth =0.6,label='1 Tm period')
plt.plot(t, interpolated_peaks, c='tab:green',linewidth = 0.8,label='Interpolated signal')
plt.plot(t, interpolated_valleys, c='tab:green',linewidth = 0.8)
plt.plot(pos, peaks_valleys, '.',c='tab:red',markersize = 0.5,label='Peaks and valleys')
plt.plot(max_pos/fm, first_max, 'D',markersize = 2,c='tab:purple',label='Maximum envelope value')
plt.plot(min_pos/fm, first_min, 'D',markersize = 2,c='tab:pink',label='Minimum envelope value')

#obtain A and fm
fm = fm/(max_pos+(min_pos-max_pos)*2)
A = first_min/first_maxs

plt.title("Estimated fm (1/Tm) = " +"{:.4f}".format(fm)+" Hz\nEstimated A (Minimum/Maximum envelope values) = "+"{:.4f}".format(A), fontsize=5)

plt.suptitle("Waveform with tremolo")
plt.ylabel("Amplitude")
plt.xlabel("Time (s)")

leg = plt.legend(loc='upper right', prop={'size': 6})
for legobj in leg.legendHandles:
    legobj.set_linewidth(1.0)