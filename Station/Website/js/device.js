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

    var trace = {
        x: [new Date()],
        y: [80],
        mode: 'lines+markers',
        name: 'Heart Rate',
        line: {shape: 'spline'},
        type: 'scatter'
    };

    var data = [trace];

    var layout = {
            xaxis: {
                type: 'date',
                title: 'Time',
                range: [new Date() - 10000, new Date()] // Set initial range to 10 seconds
            },
            yaxis: {
                title: 'Heart Rate (bpm)',
                range: [50, 120], // Heart rate range
            },
            margin: { t: 30, b: 40, l: 50, r: 10 },
            showlegend: false,
            dragmode: false,
    };

    Plotly.newPlot('heartRateChart', data, layout, { responsive: true });

    function updateHeartRateData(heartRate) {
        const now = new Date();

        Plotly.extendTraces('heartRateChart', {
            x: [[now]],
            y: [[heartRate]]
        }, [0]);

        // Keep the X-axis range sliding over the last 10 seconds
        Plotly.relayout('heartRateChart', {
            'xaxis.range': [now - 10000, now]
        });
    }

    // Mock Heart Rate data update (every second)
    setInterval(() => {
        const heartRate = Math.floor(Math.random() * 40) + 60;  // 60-100 bpm
        updateHeartRateData(heartRate);
    }, 1000);
    
    setInterval(updateSensorStatus("gps-status"), 5000);
    setInterval(updateSensorStatus("heart-status"), 5000);
    setInterval(updateSensorStatus("temperature-status"), 5000);
    setInterval(updateSensorStatus("oxygen-status"), 5000);

    function fetchData() {
        
        const temp = Math.random() * 2+ 36;
        document.getElementById('temperature-value').textContent = temp.toFixed(2);

        const oxygenSaturation = Math.random() * 2 + 3;
        document.getElementById('oxygen-value').textContent = oxygenSaturation.toFixed(2);
    }

    setInterval(fetchData, 1000);
});