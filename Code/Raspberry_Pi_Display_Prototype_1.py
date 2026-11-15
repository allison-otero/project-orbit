import serial
import struct
import time
from collections import deque

import numpy as np
import matplotlib.pyplot as plt


def calculate_features(window, sample_rate):
    centered = window - np.mean(window)
    rms = np.sqrt(np.mean(centered ** 2))

    tapered = centered * np.hanning(len(centered))
    power = np.abs(np.fft.rfft(tapered)) ** 2
    frequencies = np.fft.rfftfreq(len(centered), d=1 / sample_rate)

    keep = (frequencies >= 20) & (frequencies <= 450)
    band_power = power[keep]
    band_frequencies = frequencies[keep]
    cumulative_power = np.cumsum(band_power)

    if cumulative_power.size == 0 or cumulative_power[-1] <= 0:
        mdf = np.nan
    else:
        halfway = cumulative_power[-1] / 2
        mdf = band_frequencies[np.searchsorted(cumulative_power, halfway)]

    return rms, mdf


PORT = "COM3"
BAUD_RATE = 460800

arduino = serial.Serial(PORT, BAUD_RATE, timeout=0.1)

time.sleep(2)

print("Arduino connected successfully!")


SAMPLE_RATE = 2000

HEADER = b'\xA5\x5A'
PACKET_SIZE = 74

samples = []
sample_indices = []
buffer = bytearray()
missing_samples = 0
previous_end = None
last_overruns = None

WINDOW_SIZE = 500
pending_values = deque()
pending_indices = deque()
first_sample_index = None

# Keep the latest 120 results for the live display
live_times = deque(maxlen=120)
live_rms = deque(maxlen=120)
live_mdf = deque(maxlen=120)

fig, (rms_ax, mdf_ax) = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

plt.ion()

rms_line, = rms_ax.plot([], [])
mdf_line, = mdf_ax.plot([], [])

rms_ax.set_ylabel("RMS (V)")
mdf_ax.set_ylabel("MDF (Hz)")
mdf_ax.set_xlabel("Time (seconds)")
rms_ax.grid(True)
mdf_ax.grid(True)

fig.suptitle("ORBIT — Live RMS and MDF")
fig.tight_layout()

plt.show(block=False)

last_display_update = time.monotonic()

arduino.reset_input_buffer()

last_status = time.monotonic()

try:
    while True:
        # closing the window stops data collection
        if not plt.fignum_exists(fig.number):
            print("Plot window closed. Recording stopped.")
            break
        # Read incoming binary data
        chunk = arduino.read(arduino.in_waiting or 1)
        buffer.extend(chunk)

        # Decode complete packets
        while len(buffer) >= PACKET_SIZE:

            pos = buffer.find(HEADER)

            if pos == -1:
                # Keep the last byte in case it is A5
                buffer = buffer[-1:]
                break

            del buffer[:pos]

            if len(buffer) < PACKET_SIZE:
                break

            packet = bytes(buffer[2:PACKET_SIZE])
            start_index, overruns, *values = struct.unpack("<II32H", packet)

            # Check whether samples have been lost
            if previous_end is not None:
                if start_index != previous_end:
                    pending_values.clear()
                    pending_indices.clear()
                    if start_index > previous_end:
                        missing_samples += start_index - previous_end
                    else:
                        print("Sample index discontinuity detected.")

            samples.extend(values)
            sample_indices.extend(range(start_index, start_index + len(values)))
            if first_sample_index is None:
                first_sample_index = start_index

            pending_values.extend(values)
            pending_indices.extend(
                range(start_index, start_index + len(values))
            )

            while len(pending_values) >= WINDOW_SIZE:
                window = np.array([
                    pending_values.popleft()
                    for _ in range(WINDOW_SIZE)
                ], dtype=float)

                indices = np.array([
                    pending_indices.popleft()
                    for _ in range(WINDOW_SIZE)
                ], dtype=np.int64)

                window *= 3.3 / 4095
                rms, mdf = calculate_features(window, SAMPLE_RATE)

                midpoint = (indices[0] + indices[-1]) / 2
                live_times.append(
                    (midpoint - first_sample_index) / SAMPLE_RATE)
                live_rms.append(rms)
                live_mdf.append(mdf)
            previous_end = start_index + 32
            last_overruns = overruns

            del buffer[:PACKET_SIZE]

        now = time.monotonic()

        if live_times and now - last_display_update >= 0.25:
            rms_line.set_data(list(live_times), list(live_rms))
            mdf_line.set_data(list(live_times), list(live_mdf))

            for ax in (rms_ax, mdf_ax):
                ax.relim()
                ax.autoscale_view()

            fig.canvas.draw_idle()
            last_display_update = now

        plt.pause(0.001)

except KeyboardInterrupt:
    print("Recording stopped.")

finally:
    arduino.close()
    plt.close(fig)
          
samples = np.array(samples)
sample_indices = np.array(sample_indices, dtype=np.int64)