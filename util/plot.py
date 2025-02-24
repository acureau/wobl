import matplotlib.pyplot as plt
import numpy as np

# Path to your data file
file_path = "..\\build\\data.txt"

# Read waveform data from file
with open(file_path, "r", encoding="utf-8-sig") as file:
    waveform_data = [float(line.strip()) for line in file]

# Generate x-axis values
x_values = np.arange(len(waveform_data))

# Plot the waveform
plt.figure(figsize=(10, 4))
plt.plot(x_values, waveform_data, label="Waveform", color="b")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.title("Waveform Plot")
plt.legend()
plt.grid(True)
plt.show()
