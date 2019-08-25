from osc_gen import wavetable, dsp, sig, visualize
import numpy as np
from scipy import fftpack

# Create a signal generator.
sg = sig.SigGen()
Fs = 44100
# Create a wave table with 1024 slots to store the waves.
wt = wavetable.WaveTable(1, wave_len=512)
m = sg.saw()
wt.waves = [m]

#visualize.plot_wavetable(wt)

timestep = 1.0 / Fs
X = fftpack.rfft(wt.waves[0])
freq = fftpack.rfftfreq(X.size, d=timestep)
# 
# Make array uniform with C input data
X = np.insert(X, 0, 0)
freq = np.insert(freq, 0, 0)
X = np.delete(X, -1)
freq = np.delete(freq, -1)

outfile = "./waves/sawtooth_512.cgw"
with open(outfile, 'w') as f:
    f.write(str(len(X)) + '\n')
    for i, data in enumerate(X):    
        f.write(str(data) + '\t\t/* ' + str(freq[i]) + ' Hz  */\n')
