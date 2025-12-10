// server.js (CODE ĐÃ SỬA VÀ HOÀN CHỈNH)

const { SerialPort, ReadlineParser } = require('serialport');
const WebSocket = require('ws');

// ===============================================
// 1. Cấu hình Serial Port
// ===============================================
// ⚠️ CHỈNH SỬA TÊN CỔNG COM CỦA BẠN TẠI ĐÂY
const portName = 'COM4'; 
const baudRate = 9600;

// Khởi tạo Serial Port
const serialPort = new SerialPort({ path: portName, baudRate: baudRate });
const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

// ===============================================
// 2. Khởi tạo WebSocket Server
// ===============================================
const wss = new WebSocket.Server({ port: 3000 });
console.log(`\n==============================================`);
console.log(`WebSocket Server đang chạy trên ws://localhost:8080`);
console.log(`==============================================`);

// Xử lý kết nối mới từ Web Client
wss.on('connection', function connection(ws) {
  console.log('✅ Web Client đã kết nối!');
});

// ===============================================
// 3. Xử lý dữ liệu từ Serial và gửi qua WebSocket
// ===============================================
serialPort.on('open', () => {
  console.log(`Serial Port ${portName} đã mở. Đang chờ dữ liệu Arduino...`);
});

// Lỗi kết nối COM
serialPort.on('error', function(err) {
  console.error(`❌ Lỗi Serial Port ${portName}:`, err.message);
  console.log(`Vui lòng kiểm tra: 1. Cổng COM có đúng không? 2. Đã chạy Proteus/Arduino chưa?`);
});


parser.on('data', data => {
  const rawData = data.trim();
  
  try {
    // Dữ liệu nhận được: "T:27.50,H:82.00"
    const tempMatch = rawData.match(/T:([\d.]+)/);
    const humMatch = rawData.match(/H:([\d.]+)/);

    if (tempMatch && humMatch) {
      const temperature = parseFloat(tempMatch[1]);
      const humidity = parseFloat(humMatch[1]);
      const timestamp = new Date().toLocaleTimeString();

      const sensorData = {
        time: timestamp,
        temp: temperature,
        hum: humidity
      };

      // Gửi dữ liệu JSON đến tất cả các Client Web đang kết nối
      const jsonString = JSON.stringify(sensorData);
      
      wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(jsonString);
        }
      });
    }
  } catch (error) {
    console.error('❌ Lỗi xử lý dữ liệu hoặc gửi WebSocket:', error);
  }
});