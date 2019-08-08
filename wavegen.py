from osc_gen import wavetable, dsp, sig, visualize
import numpy as np
from scipy import fftpack

# Create a signal generator.
sg = sig.SigGen()

# Create a wave table with 1024 slots to store the waves.
wt = wavetable.WaveTable(1, wave_len=512)
m = sg.saw()
wt.waves = [m]

#visualize.plot_wavetable(wt)

X = fftpack.rfft(wt.waves[0])

# 
# Make array uniform with C input data
X = np.insert(X, 0, 0)
X = np.delete(X, -1)

outfile = "./waves/sawtooth_512.cgw"
with open(outfile, 'w') as f:
    f.write(str(len(X)) + '\n')
    for data in X:    
        f.write(str(data) + '\n')
