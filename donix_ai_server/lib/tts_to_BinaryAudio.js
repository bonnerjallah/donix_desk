import dotenv from 'dotenv';
import path from 'path';

dotenv.config({path: path.resolve(process.cwd(), '.env')});


export const ttsToBinaryAudio = async (ttsData) => {

    try {

        // Send text to TTS
        const response = await fetch('https://api.openai.com/v1/audio/speech', {
                method: 'POST',
                headers: {
                    'Authorization': `Bearer ${process.env.OPENAI_API_KEY}`,
                    'Content-Type': 'application/json'
                },

                body: JSON.stringify({
                    model: 'gpt-4o-mini-tts',
                    voice: 'alloy',
                    input: ttsData,
                    response_format: 'pcm'
                })
            }
        );


        // Check OpenAI response
        if (!response.ok) {

            const errorText = await response.text();

            throw new Error(
                `TTS API error ${response.status}: ${errorText}`
            );
        }


        // Receive audio
        const audioArrayBuffer = await response.arrayBuffer();


        // Convert audio to Node.js binary Buffer
        const binaryAudioData = Buffer.from(audioArrayBuffer);


        console.log('TTS AUDIO SIZE:', binaryAudioData.length, 'bytes');


        // Return binary audio
        return binaryAudioData;


    } catch (error) {

        console.error('Error generating TTS audio:', error);

        return null;
    }
};