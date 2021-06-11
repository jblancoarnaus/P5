import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 180
from scipy.fft import fft, ifft
import scipy.interpolate as spi
from scipy.signal import find_peaks

fm, data = wavfile.read('test_vibrato_I_025_fm_500.wav')
fm,data_I5 =wavfile.read('test_vibrato_I_5_fm_500.wav')
fm,data_I10 =wavfile.read('test_vibrato_I_10_fm_500.wav')
fm,data_I24 =wavfile.read('test_vibrato_I_24_fm_500.wav')

N=int(1024*2)
N_2=round(N/2)
x = np.array(data)
x2= np.array(data_I5)
x3= np.array(data_I10)
x4= np.array(data_I24)

input_semitones = 70
theoretical_fc = pow(2, (input_semitones-69)/12)*440
print(theoretical_fc)
theoretical_fm = 500
I = 0.25

#first_index_s = 0.2
#first_index = round(first_index_s*fm)
first_index=0
last_index = first_index+N

x = x[first_index:last_index]
x2 = x2[first_index:last_index]
x3 = x3[first_index:last_index]
x4 = x4[first_index:last_index]

#obtain fft
y = fft(x)
y = np.abs(y)

y2 = fft(x2)
y2 = np.abs(y2)

y3 = fft(x3)
y3 = np.abs(y3)

y4 = fft(x4)
y4 = np.abs(y4)

#phase = np.angle(x)
n_axis = np.linspace(0,N-1,num=N)
#convert axis to kHz
f_axis = n_axis/N*fm/1000  


#find peaks
state = 0
state_prev = 0
peaks_array = []
peaks_pos_array = []
for i in range(len(y[0:N_2])):

  if (y[i+1]-y[i]) > 3:
    state = 1
  else:
    state = -1

  #if a peak has been found, store its amplitude and index
  if state_prev == 1 and state == -1:
    peaks_array.append(y[i])
    peaks_pos_array.append(i)

  state_prev = state

num_peaks = 3 # # of maximas to find
peaks_found=[]
peaks_found_pos=[]
y_temp = y[0:N_2].copy()

for i in range(num_peaks):
  current_max=np.max(peaks_array)
  peaks_found_pos_array=peaks_pos_array[np.argmax(peaks_array)]
  current_max_pos = peaks_found_pos_array
  peaks_array[np.argmax(peaks_array)]=0

  peaks_found.append(current_max)
  peaks_found_pos.append(current_max_pos)

for i in range(len(peaks_found_pos)):
  peaks_found_pos[i]=peaks_found_pos[i]*(fm/N)/1000 #convert axis to kHz


num=8
ini=0
plt.plot(f_axis[ini:int(N_2/num)],y[ini:int(N_2/num)], c='tab:blue',linewidth = 0.4,label='FFT I = 0.25')
plt.plot(f_axis[ini:int(N_2/num)],y2[ini:int(N_2/num)], c='tab:green',linewidth = 0.4,label='FFT I = 5')
plt.plot(f_axis[ini:int(N_2/num)],y3[ini:int(N_2/num)], c='tab:orange',linewidth = 0.4,label='FFT I = 10')
plt.plot(f_axis[ini:int(N_2/num)],y4[ini:int(N_2/num)], c='tab:pink',linewidth = 0.4,label='FFT I = 24')


#theoretical values
harmonics=[]
harmonics_amplitude=[]
num_harmonics = 2
for i in range(num_harmonics):
  harmonic_pos_prev=theoretical_fc-(i+1)*theoretical_fm
  harmonic_pos_aft=theoretical_fc+(i+1)*theoretical_fm
  if(harmonic_pos_prev>0):
    harmonics.append(round(harmonic_pos_prev*N/fm))
    harmonics_amplitude.append(y[int(round(harmonic_pos_prev*N/fm))])
  if(harmonic_pos_aft<fm*N_2):
    harmonics.append(round(harmonic_pos_aft*N/fm))
    harmonics_amplitude.append(y[int(round(harmonic_pos_aft*N/fm))])

#plt.plot(f_axis[round(theoretical_fc*N/fm)],y[round(theoretical_fc*N/fm)],'D', c='tab:green',markersize = 2.5,label='Theoretical fc')
#plt.plot(f_axis[harmonics],y[harmonics],'D', c='tab:red',markersize = 2.5,label='Theoretical fm')
#plt.plot(phase[0:N], c='tab:red',linewidth = 0.6,label='Phase')
#plt.plot( x, c='tab:blue',linewidth = 0.25,label='Signal')

plt.plot(peaks_found_pos[0],peaks_found[0],'D', c='tab:red',markersize = 2.5,label='Fc peak')
#plt.plot(peaks_found_pos[1:],peaks_found[1:],'D', c='tab:orange',markersize = 2.5,label='Fm peaks')
fc_hz = peaks_found_pos[0]*1000
fm_hz =  abs(peaks_found_pos[1]*1000-fc_hz)

plt.suptitle("FFT of a signal with vibrato")
#plt.title("Input parameters:  fm = " +"{:.2f}".format(theoretical_fm) +" Hz;  fc =" +"{:.2f}".format(theoretical_fc) +"Hz;  I = "+"{:.0f}".format(I)+ " semitone(s)\nfm estimate = " +"{:.2f}".format(fm_hz)+" Hz;  fc estimate = "+"{:.2f}".format(fc_hz)+" Hz", fontsize=5)
#plt.title("Input parameters:  fm = " +"{:.2f}".format(theoretical_fm) +" Hz;  fc =" +"{:.2f}".format(theoretical_fc) +"Hz;  I = "+"{:.2f}".format(I)+ " semitone(s)", fontsize=6)
plt.title("Input parameters:  fm = " +"{:.2f}".format(theoretical_fm) +" Hz;  fc =" +"{:.2f}".format(theoretical_fc) +"Hz", fontsize=6)

plt.ylabel("Amplitude")
plt.xlabel("Frequency (kHz)")

peaks_fft = find_peaks(y)
leg = plt.legend(loc='upper right', prop={'size': 8})
for legobj in leg.legendHandles:
    legobj.set_linewidth(1.5)