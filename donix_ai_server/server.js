import dotenv from 'dotenv';
import path from 'path';
import express from 'express';
import http from 'http';
import { WebSocketServer } from 'ws';

dotenv.config({ path: path.resolve(process.cwd(), '.env')});

const app = express();

const server = http.createServer(app);

const PORT = process.env.PORT || 3001;

// =========================
// Express Routes
// =========================

app.get('/', (req, res) => {
    res.send('DONIX AI Server is running!');
});

// =========================
// WebSocket Server
// =========================

const wss = new WebSocketServer({
  server, path: '/ws'
});

wss.on('connection', (socket) => {

  console.log('ESP32 WebSocket connected');

  socket.send(JSON.stringify({
    type: 'connection',
    message: 'Hello from DONIX server!'
  }));

  socket.on('message', (data, isBinary) => {
    
    if(!isBinary){
      return;
    }

    console.log('Received binary data from ESP32:', data);
    console.log('Received binary data length:', data.length);

    const sample = [];

    for(let i = 0; i < data.length; i += 4) {
      const value = data.readInt16LE(i);
      sample.push(value);
    }

    console.log('Parsed sample data:', sample);
    console.log("First 10 values of sample data:", sample.slice(0, 10));

  });

  socket.on('close', () => {
    console.log('ESP32 WebSocket disconnected');
  });

  socket.on('error', (error) => {
    console.error('WebSocket error:', error);
  });

});

// =========================
// Start Server
// =========================

server.listen(PORT, () => {
  console.log(`DONIX AI Server is running on port ${PORT}`);
});