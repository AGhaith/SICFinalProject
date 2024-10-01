import serial

# Open serial port (adjust the port name as needed)
ser = serial.Serial('/dev/ttyACM1', 9600, timeout=1)  # Adjust the port name if necessary
while True:
    # Read the serial data line by line
    line = ser.readline().decode('utf-8').strip()

    if line.startswith("LAT"):
        lat = float(line.split(":")[1].strip())
        print(f"Latitude: {lat}")

    elif line.startswith("LONG"):
        long = float(line.split(":")[1].strip())
        print(f"Longitude: {long}")

    elif line.startswith("Temperature"):
        temperature = float(line.split(":")[1].strip())
        print(f"Temperature: {temperature}")
    elif line.startswith("UV"):
        uv = float(line.split(":")[1].strip())
        print(f"UltraViolet: {uv}")
    else :
	    print(line)

