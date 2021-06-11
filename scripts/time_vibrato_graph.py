import numpy as np
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 170
plt.rcParams.update({'font.size': 9})


x = np.array(data)
x2= np.array(data_I5)
x3= np.array(data_I10)
x4= np.array(data_I24)
first_index=0
last_index=3500
index_num = last_index-first_index
x = x[first_index:last_index]
x2 = x2[first_index:last_index]
x3 = x3[first_index:last_index]
x4 = x4[first_index:last_index]

fig, axs = plt.subplots(4)
plt.tight_layout()
fig.suptitle("Waveform with vibrato", y=1.02)

t_axis =  np.linspace(0,(N-1)/fm,num=index_num)
axs[0].plot(t_axis[first_index:last_index], x,c='tab:blue',label='FFT I = 0.25', linewidth=0.7)
axs[1].plot(t_axis[first_index:last_index],x2,c='tab:green',label='FFT I = 5', linewidth=0.7)
axs[2].plot(t_axis[first_index:last_index],x3, c='tab:orange',label='FFT I = 10', linewidth=0.7)
axs[3].plot(t_axis[first_index:last_index],x4, c='tab:pink',label='FFT I = 24', linewidth=0.7)

for i in range(4):
    axs[i].set_ylabel("Amplitude")
    axs[i].set_xlabel("Time (s)")
    axs[i].legend(loc='upper right', prop={'size': 6})

plt.show()