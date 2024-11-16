import machine
import _thread
import array

# Constants
ADC_PIN = 26  # ADC0 on GP26
BUFFER_SIZE = 4096  # Size of each buffer
REFERENCE_VOLTAGE = 3.3  # ADC reference voltage
ADC_RESOLUTION = 65535  # 16-bit resolution

# Shared Buffers for Double Buffering
buffer1 = array.array('H', [0] * BUFFER_SIZE)
buffer2 = array.array('H', [0] * BUFFER_SIZE)

# Flags and synchronization
buffer_ready = False
current_buffer = 0
lock = _thread.allocate_lock()

# Function to capture ADC samples at maximum speed (Core 0)
def adc_sampling():
    global buffer_ready, current_buffer

    adc = machine.ADC(machine.Pin(ADC_PIN))

    while True:
        # Choose the current buffer for writing
        buffer = buffer1 if current_buffer == 0 else buffer2

        # Fill the buffer with ADC readings
        for i in range(BUFFER_SIZE):
            buffer[i] = adc.read_u16()

        # Signal the other core that the buffer is ready
        with lock:
            buffer_ready = True

        # Swap to the other buffer
        current_buffer = 1 - current_buffer

# Function to print ADC data continuously (Core 1)
def print_data():
    global buffer_ready, current_buffer

    while True:
        # Wait until a buffer is ready
        if buffer_ready:
            # Determine which buffer to read
            buffer = buffer2 if current_buffer == 0 else buffer1

            # Print the ADC values as voltages
            for value in buffer:
                voltage = int(value * (100 / ADC_RESOLUTION)) 
                print(voltage)

            # Mark the buffer as processed
            with lock:
                buffer_ready = False

# Start the ADC sampling on Core 0
_thread.start_new_thread(adc_sampling, ())

# Print data on Core 1 (Main Core)
print_data()

