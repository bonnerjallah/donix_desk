import dotenv from 'dotenv';
import path from 'path';
import { File } from 'node:buffer';
import { ttsToBinaryAudio } from '../lib/tts_to_BinaryAudio.js';

dotenv.config({path: path.resolve(process.cwd(), '.env')});

import {generateAIResponse} from '../services/generate_ai_response.js';
import { createWavFile } from '../lib/createWavFile.js';


// =====================================================
// Send audio to OpenAI
// =====================================================

export const sendToOpenAI = async (pcmData) => {

  let userInput = '';

  try {

    // -------------------------------------------------
    // Create WAV
    // -------------------------------------------------

    const wavData = createWavFile(pcmData, 16000, 1, 16);

    const form = new FormData();

    const audioFile = new File( [wavData], 'recording.wav', {
      type: 'audio/wav'
    });


    form.append('file', audioFile);

    form.append('model', 'gpt-4o-mini-transcribe');

    const response = await fetch('https://api.openai.com/v1/audio/transcriptions', {
      method: 'POST',
      headers: {'Authorization': `Bearer ${process.env.OPENAI_API_KEY}`},
      body: form
    });

    const responseText = await response.text();

    if (!response.ok) {
      throw new Error( `OpenAI API error ${response.status}: ${responseText}`);
    }

    const data = JSON.parse(responseText);

    console.log('FULL TRANSCRIPTION RESPONSE:', data);

    userInput = data.text || '';

    console.log('OpenAI transcription result:', userInput);


  } catch (error) {

    console.error('Error sending audio to OpenAI:', error);

    return null;
  }

  // -------------------------------------------------
  // Generate AI response
  // -------------------------------------------------

  if (!userInput.trim()) {
    console.log('No speech detected. Skipping AI response.');
    return null;
  }

  const aiResponse = await generateAIResponse(userInput);

  if (!aiResponse) {
    console.error('Failed to generate AI response');
    return null;
  }

  // -------------------------------------------------
  // Convert AI response to binary audio
  // -------------------------------------------------

  const binaryAudio = await ttsToBinaryAudio(aiResponse);

  if (!binaryAudio) {
    console.error('Failed to convert AI response to binary audio');
    return null;
  }

  return { aiResponse, binaryAudio };
};

export const sendToEsp32 = (socket, binaryAudio) => {

  if (!socket || !binaryAudio) {
    console.error('Invalid socket or binary audio');
    return;
  }

  const CHUNK_SIZE = 2048;

  console.log(
    'Sending TTS audio:',
    binaryAudio.length,
    'bytes'
  );

  for (let offset = 0; offset < binaryAudio.length; offset += CHUNK_SIZE) {

    const chunk = binaryAudio.subarray(
      offset,
      Math.min(
        offset + CHUNK_SIZE,
        binaryAudio.length
      )
    );

    socket.send(chunk);
  }

  console.log(
    'Finished sending TTS audio'
  );
};