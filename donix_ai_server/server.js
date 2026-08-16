import dotenv from 'dotenv';
import path from 'path';
import express from 'express';
import http from 'http';
import { WebSocketServer } from 'ws';

dotenv.config({path: path.resolve(process.cwd(), '.env')});

import {sendToOpenAI} from './api/open_ai.js';


const app = express();

const server = http.createServer(app);

const PORT = process.env.PORT || 3001;



// =========================
// WebSocket Server
// =========================

const wss = new WebSocketServer({
  server,
  path: '/ws'
});


wss.on('connection', (socket) => {

  console.log('ESP32 WebSocket connected');

  socket.send(JSON.stringify({
    type: 'connection',
    message: 'Hello from DONIX server!'
  }));


  let audioChunks = [];

  socket.on('message', (data, isBinary) => {

    console.log(
      'WS MESSAGE RECEIVED',
      'isBinary:', isBinary,
      'length:', data.length
    );

    if (!isBinary) {

      const message = data.toString();

      console.log('TEXT:', message);

      if (message === 'RECORDING_COMPLETE') {

        const pcmData = Buffer.concat(audioChunks);
        
        console.log('RECORDING COMPLETE, PCM DATA LENGTH:', pcmData.length);

        audioChunks = [];

        sendToOpenAI(pcmData)
      }

      return;
    }

    console.log('BINARY AUDIO:', data.length);

    audioChunks.push(Buffer.from(data));
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

server.listen(PORT, '0.0.0.0', () => {
  console.log(`DONIX AI Server is running on port ${PORT}`);
});