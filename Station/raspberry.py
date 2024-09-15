import serial
ser = serial.Serial('/dev/ttyS0', 9600, timeout=1)  # Adjust the port name if necessary
delimiter = '\r\n\r\n'
buffer = ""

def recievedata():
    global buffer
    while True:
        if ser.in_waiting > 0:
            buffer += ser.read(ser.in_waiting).decode('utf-8')
            
            # Check if the delimiter is in the buffer
            if delimiter in buffer:
                # Extract the message up to the delimiter
                message, buffer = buffer.split(delimiter, 1)
                print(f"Received")
                return message

while True:
    alldata = recievedata().split()
    if len(alldata) >= 9:
        lat = alldata[2]
        long = alldata[5]
        temp = alldata[8]
        print(lat, long, temp)
    else:
        print("Data format is incorrect.")