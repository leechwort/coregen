from osc_gen import wavetable, dsp, sig, visualize
import numpy as np
from scipy import fftpack
from scipy.io.wavfile import write
import matplotlib.pyplot as plt

# Create a signal generator.
sample_length = 2048
sg = sig.SigGen(num_points=sample_length)
Fs = 44100

# Create a wave table with 1024 slots to store the waves.
wt = wavetable.WaveTable(1, wave_len=1024)
#m = sg.saw()
m = sg.tri()
wt.waves = [m]

#visualize.plot_wavetable(wt)

timestep = 1.0 / Fs
rFFT_values = fftpack.rfft(m)
freq = fftpack.rfftfreq(rFFT_values.size, d=timestep)

# Make array uniform with C input data
X = rFFT_values
X = np.insert(X, 0, 0)
freq = np.insert(freq, 0, 0)
X = np.delete(X, -1)
freq = np.delete(freq, -1)

outfile = "./waves/tri_%d.cgw" % sample_length
with open(outfile, 'w') as f:
    f.write(str(len(m)) + '\n')
    for i, data in enumerate(m):    
        #f.write(str(data) + '    # ' + str(freq[i]) + ' Hz\n')
        f.write(str(data) + '\n')


plt.plot(m, 'r.-')


