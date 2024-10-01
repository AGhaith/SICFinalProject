import tkinter as tk
from tkinter import ttk
import serial
import asyncio
import websockets

ser = serial.Serial('COM14', 9600, timeout=1)  # Adjust to your COM port

# Send the command to the serial device
def send_command(command):
    ser.write(command.encode())

# Define the WebSocket handler
async def echo(websocket, path):
    print("Client connected")
    try:
        async for message in websocket:
            print(f"Received message: {message}")
            send_command(message)
            # Echo the message back to the client
            await websocket.send(f"Server received: {message}")
    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected")

# Start the WebSocket server
async def main():
    async with websockets.serve(echo, "0.0.0.0", 9090):
        print("WebSocket server started on ws://0.0.0.0:9090")
        await asyncio.Future()  # Run forever

# Start the event loop
if __name__ == "__main__":
    asyncio.run(main())
