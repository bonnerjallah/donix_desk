
 export const createWavFile = ( pcmData, sampleRate, channels, bitsPerSample) => {

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

