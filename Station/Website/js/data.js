const dataHistory = [];

function updateDataHistory(heartRate, oxygenSaturation) {
    const now = new Date().toLocaleString();
    dataHistory.push({ timestamp: now, heartRate, oxygenSaturation });
    
    // Update the table
    const dataBody = document.getElementById('data-body');
    dataBody.innerHTML = ''; // Clear existing rows
    dataHistory.forEach(data => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${data.timestamp}</td>
            <td>${data.heartRate}</td>
            <td>${data.oxygenSaturation}</td>
        `;
        dataBody.appendChild(row);
    });
}

setInterval(updateDataHistory(Math.floor(Math.random() * 40) + 60, Math.random() * 10 + 90), 1000);