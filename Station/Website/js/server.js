const WebSocket = require('ws');

// Create a WebSocket server listening on port 8080
const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => {
    console.log('Client connected');

    // When a message is received from the client
    ws.on('message', (message) => {
        console.log(`Received: ${message}`);
    });

    // Send a message back to the client
    ws.send('Hello from Raspberry Pi');
});

console.log('WebSocket server running on ws://localhost:8080');