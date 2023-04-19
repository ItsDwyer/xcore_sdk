import math
import wave

sample_rate = 16000
duration = 5
frequency = 700
amplitude = 32767

wav_file = wave.open("test_tone_700hz.wav", "w")
wav_file.setparams((1, 2, sample_rate, sample_rate*duration, "NONE", "not compressed"))

for i in range(sample_rate*duration):
    sample = int(amplitude*math.sin(2*math.pi*frequency*(i/sample_rate)))
    wav_file.writeframesraw(sample.to_bytes(2, byteorder="little", signed=True))

wav_file.close()