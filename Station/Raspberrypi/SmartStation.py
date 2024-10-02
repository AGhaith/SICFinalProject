from gpiozero import DistanceSensor, Servo, LED
from gpiozero.pins.pigpio import PiGPIOFactory
from time import sleep
import RPi.GPIO as GPIO
import smbus
import time
import asyncio
import websockets
import json

# Define L3G4200D registers and addresses
L3G4200D_ADDRESS = 0x69
L3G4200D_CTRL_REG1 = 0x20
L3G4200D_OUT_X_L = 0x28

# Initialize the I2C bus for the gyroscope
bus = smbus.SMBus(1)

# Set up the ultrasonic sensor (adjust GPIO pins as needed)
factory = PiGPIOFactory()
sensor = DistanceSensor(echo=24, trigger=25, pin_factory=factory)
ultrasonic1 = DistanceSensor(echo=20, trigger=21)
led2 = LED(26)
led3 = LED(6)
led4 = LED(5)

# Set up the servo (adjust GPIO pin as needed)
servo = Servo(18, min_pulse_width=0.45/1000, max_pulse_width=2.5/1000, pin_factory=factory)

# Setup GPIO for buzzer, LED, and flame sensor
BUZZER_PIN = 23
LED_PIN = 22  # GPIO pin connected to the additional LED
FLAME_SENSOR_PIN = 27  # GPIO pin connected to the flame sensor

GPIO.setmode(GPIO.BCM)
GPIO.setup(BUZZER_PIN, GPIO.OUT)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.setup(FLAME_SENSOR_PIN, GPIO.IN)  # Set flame sensor pin as input

# Function to control buzzer and LED
def activate_alerts(buzzer_state, led_state):
    trigger_buzzer(buzzer_state)
    control_led1(led_state)

# Function to control the buzzer
def trigger_buzzer(state):
    GPIO.output(BUZZER_PIN, GPIO.HIGH if state == "ON" else GPIO.LOW)

# Function to control an additional LED
def control_led1(state):
    GPIO.output(LED_PIN, GPIO.HIGH if state == "ON" else GPIO.LOW)

# Function to check the flame sensor
def check_flame():
    # Read data from the flame sensor (active low)
    flame_detected = GPIO.input(FLAME_SENSOR_PIN)
  
    # Activate alerts if a flame is detected
    if flame_detected:
        print("Flame Detected")
        activate_alerts("ON", "ON")
    else:
        print("No flame detected")
        activate_alerts("OFF", "OFF")

# Function to map distance (from ultrasonic sensor) to servo angle
def map_distance_to_angle(distance):
    # Map the distance (in meters) to a servo value (-1 to 1)
    if distance < 0.1:
        return 0
    elif distance > 0.1:
        return -1

# Function to control the LED based on distance
def control_led(dist):
    if dist < 0.35:
        led2.on()
        led3.on()
        led4.on()
        print("LED on")
    else:
        led2.off()
        led3.off()
        led4.off()
        print("LED off")

# Initialize the gyroscope
def initialize_gyro():
    # Enable the gyroscope (normal mode, all axes enabled, 100 Hz)
    bus.write_byte_data(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, 0x0F)

# Read raw data from the gyroscope
def read_raw_data(register):
    low = bus.read_byte_data(L3G4200D_ADDRESS, register)
    high = bus.read_byte_data(L3G4200D_ADDRESS, register + 1)
    value = (high << 8) + low
    if value > 32767:
        value -= 65536
    return value

# Get gyroscope data (X, Y, Z)
def get_gyro_data():
    x = read_raw_data(L3G4200D_OUT_X_L)
    y = read_raw_data(L3G4200D_OUT_X_L + 2)
    z = read_raw_data(L3G4200D_OUT_X_L + 4)
    return x, y, z

# Function to collect all sensor data
def get_all_sensor_data():
    # Read distances from the ultrasonic sensors
    distance1 = sensor.distance
    distance2 = ultrasonic1.distance

    # Read gyroscope data
    x, y, z = get_gyro_data()

    # Check the flame sensor
    flame_detected = GPIO.input(FLAME_SENSOR_PIN)

    # Create a dictionary to store all sensor data
    data = {
        "distance1": distance1,
        "distance2": distance2,
        "gyro": {"x": x, "y": y, "z": z},
        "flame_detected": flame_detected,
    }

    return data

# Define the WebSocket handler
async def echo(websocket, path):
    print("Client connected")
    try:
        async for message in websocket:
            print(f"Received message: {message}")

            # Control the servo based on received message
            try:
                servo.value = float(message)  # Convert to float if message is an angle value
            except ValueError:
                print("Invalid message for servo control")

            # Get all sensor data
            sensor_data = get_all_sensor_data()

            # Send sensor data as JSON to the client
            await websocket.send(json.dumps(sensor_data))

    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected")

# Start the WebSocket server
async def main():
    async with websockets.serve(echo, "0.0.0.0", 8080):
        print("WebSocket server started on ws://0.0.0.0:8080")
        await asyncio.Future()  # Run forever

# Start the event loop
if _name_ == "_main_":
    asyncio.run(main())

# Main loop for ultrasonic sensors, servo, flame sensor, and gyroscope
try:
    initialize_gyro()  # Initialize the gyroscope
    print("Gyroscope initialized. Monitoring sensors...")
    
    while True:
        # Read distance from ultrasonic sensors
        distance = sensor.distance  # distance from sensor 1 (in meters)
        dist = ultrasonic1.distance  # distance from sensor 2
        
        # Print distance and control LED based on distance
        print(f"Distance 1: {distance:.2f} m")
        print(f"Distance 2: {dist:.2f} m")
        control_led(dist)

        # Convert distance to servo angle
        angle = map_distance_to_angle(distance)
        servo.value = angle  # Set servo position
        #print(f"Servo angle: {angle:.2f}")

        # Check the flame sensor
        check_flame()

# Get gyroscope data
        x, y, z = get_gyro_data()
        
        # Earthquake detection (threshold for vibrations)
        if abs(x) > 1000 or abs(y) > 1000 or abs(z) > 1000:
            print("zelzal ya ostaz 3omar zelzaaaal")
            # You can trigger an alert or take any action here
        else:
            print("Eldar aman")

        # Small delay for stability
        sleep(0.25)

except KeyboardInterrupt:
    print("Program stopped by the user")

finally:
    servo.value = -1  # Reset servo to neutral position
    GPIO.cleanup()  # Cleanup GPIO pins