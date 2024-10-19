import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

# Spesifikasi filter
fc_low = 250  # Frekuensi cutoff low-pass 250 Hz
fc_high = 3000  # Frekuensi cutoff high-pass 3000 Hz
fs = 44100  # Frekuensi sampling (Hz)

# Desain filter Butterworth bandpass
b, a = signal.butter(4, [fc_low/(0.5*fs), fc_high/(0.5*fs)], btype='band')

# Frekuensi dan respon filter
w, h = signal.freqz(b, a, worN=2000)
frequencies = (fs * 0.5 / np.pi) * w

# Plot respon frekuensi
plt.figure()
plt.plot(frequencies, 20 * np.log10(abs(h)), 'b')
plt.title('Respon Frekuensi Filter Bandpass')
plt.xlabel('Frekuensi (Hz)')
plt.ylabel('Gain (dB)')
plt.xscale('log')
plt.xlim([100, 5000])  # Fokus pada area 100Hz hingga 5000Hz
plt.grid(which='both', axis='both')
plt.axvline(fc_low, color='green')  # Frekuensi cutoff 250 Hz
plt.axvline(fc_high, color='red')   # Frekuensi cutoff 3 kHz
# Ubah ticks (label) pada sumbu x
plt.xticks([100, 250, 500, 1000, 3000, 5000], 
           ['100 Hz', '250 Hz', '500 Hz', '1 kHz', '3 kHz', '5 kHz'])

# Tambahkan label dan legenda
plt.legend()
plt.show()
