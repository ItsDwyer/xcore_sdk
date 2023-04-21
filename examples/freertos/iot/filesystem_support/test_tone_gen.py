import math
import wave

sample_rate = 16000
channels = 1
sample_width = 1
duration = 5
frequency = 700
amplitude = 32767

wav_file = wave.open("test_tone_700hz.wav", "w")
wav_file.setparams((channels, sample_width, sample_rate, sample_rate*duration, "NONE", "not compressed"))

for i in range(sample_rate*duration):
    sample = int(amplitude*math.sin(2*math.pi*frequency*(i/sample_rate)))
    wav_file.writeframesraw(sample.to_bytes(1, byteorder="little", signed=True))

wav_file.close()