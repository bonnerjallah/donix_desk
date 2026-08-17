import dotenv from 'dotenv';
import path from 'path';
import { File } from 'node:buffer';

dotenv.config({
  path: path.resolve(process.cwd(), '.env')
});

import { aiRules } from '../services/ai_rules.js';

const OPENAI_API_KEY = process.env.OPENAI_API_KEY;


// =====================================================
// Create WAV file in memory
// =====================================================

const createWavFile = ( pcmData, sampleRate, channels, bitsPerSample) => {

  const header = Buffer.alloc(44);

  const byteRate = sampleRate * channels * bitsPerSample / 8;

  const blockAlign = channels * bitsPerSample / 8;


  // RIFF
  header.write('RIFF', 0);

  // File size
  header.writeUInt32LE(36 + pcmData.length, 4);

  // WAVE
  header.write('WAVE', 8);

  // fmt
  header.write('fmt ', 12);

  // PCM format
  header.writeUInt32LE(16, 16);

  header.writeUInt16LE(1, 20);

  // Channels
  header.writeUInt16LE(channels, 22);

  // Sample rate
  header.writeUInt32LE(sampleRate, 24);

  // Byte rate
  header.writeUInt32LE(byteRate, 28);

  // Block align
  header.writeUInt16LE(blockAlign, 32);

  // Bits per sample
  header.writeUInt16LE(bitsPerSample, 34);

  // data
  header.write('data', 36);

  // PCM data size
  header.writeUInt32LE(pcmData.length, 40);


  return Buffer.concat([
    header,
    pcmData
  ]);
};


// =====================================================
// Send audio to OpenAI
// =====================================================

export const sendToOpenAI = async (pcmData) => {

  try {

    // -------------------------------------------------
    // Create WAV
    // -------------------------------------------------

    const wavData = createWavFile(pcmData, 16000, 1, 16);


    // -------------------------------------------------
    // Create multipart form
    // -------------------------------------------------

    const form = new FormData();


    const audioFile = new File( [wavData], 'recording.wav', {
      type: 'audio/wav'
    });


    form.append('file', audioFile);

    form.append('model', 'gpt-4o-mini-transcribe');


    // -------------------------------------------------
    // Send to OpenAI transcription endpoint
    // -------------------------------------------------

    const response = await fetch('https://api.openai.com/v1/audio/transcriptions', {
      method: 'POST',
      headers: {'Authorization': `Bearer ${OPENAI_API_KEY}`},
      body: form
    });


    // -------------------------------------------------
    // Read response
    // -------------------------------------------------

    const responseText = await response.text();


    // -------------------------------------------------
    // Handle API errors
    // -------------------------------------------------

    if (!response.ok) {
      throw new Error( `OpenAI API error ${response.status}: ${responseText}`);
    }


    // -------------------------------------------------
    // Parse response
    // -------------------------------------------------

    const data = JSON.parse(responseText);


    console.log('TRANSCRIPTION:', data.text);


    // -------------------------------------------------
    // Return transcription
    // -------------------------------------------------

    return data.text;


  } catch (error) {

    console.error('Error sending audio to OpenAI:', error);

    return null;
  }
};