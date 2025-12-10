// app.js

const WS_SERVER_URL = 'ws://localhost:8080'; 
const maxDataPoints = 15; // Giới hạn 15 điểm dữ liệu trên biểu đồ
let tempChart;

document.addEventListener('DOMContentLoaded', () => {
    // Khởi tạo biểu đồ khi DOM đã load xong
    initializeChart();
    // Khởi tạo kết nối WebSocket
    initializeWebSocket();
});


function initializeChart() {
    const ctx = document.getElementById('tempChart').getContext('2d');
    tempChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [], 
            datasets: [{
                label: 'Nhiệt độ (°C)',
                data: [], 
                borderColor: '#dc3545', // Màu đỏ
                backgroundColor: 'rgba(220, 53, 69, 0.1)',
                borderWidth: 2,
                tension: 0.3,
                pointRadius: 4
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: {
                    title: { display: true, text: 'Nhiệt độ (°C)' },
                    beginAtZero: false
                }
            },
            plugins: {
                legend: { display: true }
            }
        }
    });
}

function initializeWebSocket() {
    const ws = new WebSocket(WS_SERVER_URL); 

    ws.onopen = () => {
        console.log('✅ Connected to WebSocket Server.');
    };

    ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data); 
            
            // Cập nhật DOM
            document.getElementById('currentTemp').innerText = data.temp.toFixed(1);
            document.getElementById('currentHum').innerText = data.hum.toFixed(1);

            // Cập nhật biểu đồ
            updateChart(data.time, data.temp);

        } catch (error) {
            console.error("❌ Error processing WebSocket message:", error);
        }
    };

    ws.onclose = () => {
        console.log('🛑 Disconnected from WebSocket Server. Trying to reconnect...');
        // Có thể thêm logic reconnect tại đây
    };

    ws.onerror = (error) => {
        console.error('Lỗi WebSocket:', error);
    };
}

function updateChart(time, temperature) {
    if (!tempChart) return;

    // Thêm dữ liệu mới
    tempChart.data.labels.push(time);
    tempChart.data.datasets[0].data.push(temperature);
    
    // Giới hạn số điểm dữ liệu
    if (tempChart.data.labels.length > maxDataPoints) {
        tempChart.data.labels.shift();
        tempChart.data.datasets[0].data.shift();
    }
    
    // Render lại biểu đồ
    tempChart.update();
}