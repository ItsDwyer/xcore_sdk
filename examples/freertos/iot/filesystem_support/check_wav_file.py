import wave

filepath = input("Enter the file path: ")

with wave.open(filepath, 'rb') as wave_file:
    num_channels = wave_file.getnchannels()
    sample_width = wave_file.getsampwidth()
    sample_rate = wave_file.getframerate()
    total_frames = wave_file.getnframes()
    print(f"num_channels = {num_channels}")
    print(f"sample_width = {sample_width}")
    print(f"sample_rate = {sample_rate}")
    print(f"total_frames = {total_frames}")

