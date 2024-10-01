import { updateSensorStatus } from './main.js';

document.addEventListener('DOMContentLoaded', function () {
   
    // Connect to the WebSocket server running on Raspberry Pi
    const socket = new WebSocket('ws://192.168.244.50:8080');

    // Event listener for when the connection is opened
    socket.addEventListener('open', (event) => {
        console.log('Connected to WebSocket server');
    });

    // Event listener for when a message is received from the server
    socket.addEventListener('message', (event) => {
        console.log('Message from server:', event.data);
    });

    document.getElementById("open").addEventListener('click', () => {
        socket.send('-1');
        console.log('Sent: -1');
    });

    document.getElementById("close").addEventListener('click', () => {
        socket.send('0');
        console.log('Sent: 0');
    });

});