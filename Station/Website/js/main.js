export function updateSensorStatus(id) {
    // Simulate sensor status check
    const random = Math.random() > .1; // 90% chance it's active

    document.getElementById(id).textContent = random ? 'Active' : 'Inactive';
    document.getElementById(id).className = random ? 'status active' : 'status inactive';
}

document.addEventListener('DOMContentLoaded', function () {

    function showAlert(message) {
        const alertsDiv = document.getElementById('alerts');
        const alertDiv = document.createElement('div');
        alertDiv.classList.add('alert');
        alertDiv.textContent = message;
        alertsDiv.appendChild(alertDiv);
        
        // Auto-remove alert after 5 seconds
        setTimeout(() => {
            alertsDiv.removeChild(alertDiv);
        }, 5000);
    }
    
    // Example: Trigger alerts based on heart rate and oxygen saturation thresholds
    function checkThresholds(heartRate, oxygenSaturation) {
        if (heartRate > 100) {
            showAlert(`Warning: High Heart Rate detected (${heartRate} bpm)!`);
        }
        if (oxygenSaturation < 90) {
            showAlert(`Warning: Low Blood Oxygen Saturation (${oxygenSaturation}%)!`);
        }
    }    

    const menuBtn = document.getElementById("menu-btn");

    menuBtn.addEventListener('click', () => {
        document.getElementById("menu").classList.toggle('active');
    });

});
