import { updateSensorStatus } from './main.js';

document.addEventListener('DOMContentLoaded', function () {
    // Initialize Leaflet map
    const map = L.map('map').setView([39.7392, -104.9903], 13); 

    // Add OpenStreetMap tiles
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(map);

    // Create a marker for the GPS location
    const marker = L.marker([39.7392, -104.9903]).addTo(map);
    
    // Function to update GPS location
    function updateGPS(lat, lon) {
        marker.setLatLng([lat, lon]);
        map.setView([lat, lon], 13);
    }

    // Mock GPS data update
    setInterval(() => {
        // Randomize coordinates for testing purposes (replace with actual GPS data)
        const lat = 39.7392 + (Math.random() * 0.01);
        const lon = -104.9903 + (Math.random() * 0.01);
        updateGPS(lat, lon);
    }, 5000);

    setInterval(updateSensorStatus("gps-status"), 5000);        

    // Connect to the WebSocket server running on Raspberry Pi
    const socket = new WebSocket('ws://192.168.1.113:9090/ws/sensor');

    // Event listener for when the connection is opened
    socket.addEventListener('open', (event) => {
        console.log('Connected to WebSocket server');
    });

    // Event listener for when a message is received from the server
    socket.addEventListener('message', (event) => {
        console.log('Message from server:', event.data);
    });

    let keysHeld = {
        ArrowUp: false,
        ArrowDown: false,
        ArrowLeft: false,
        ArrowRight: false,
        KeyW: false,
        KeyA: false,
        KeyS: false,
        KeyD: false
    };

    document.addEventListener('keydown', (event) => {
        if (event.code in keysHeld && !keysHeld[event.code]) {
            keysHeld[event.code] = true;
            document.getElementById(event.code).classList.add("held");
            
            switch(event.code) {
                case "ArrowUp":
                    socket.send('B');
                    console.log('Sent: B');
                break;

                case "ArrowDown":
                    socket.send('F');
                    console.log('Sent: F');
                break;

                case "ArrowRight":
                    socket.send('R');
                    console.log('Sent: R');
                break;

                case "ArrowLeft":
                    socket.send('L');
                    console.log('Sent: L');
                break;
            }
        }
    });

    document.addEventListener('keyup', (event) => {
        if (event.code in keysHeld) {
            keysHeld[event.code] = false;
            socket.send('S');
            console.log('Sent: S');
            document.getElementById(event.code).classList.remove("held");
        }
    });

});