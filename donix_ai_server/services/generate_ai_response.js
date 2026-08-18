import dotenv from 'dotenv';
import path from 'path';

dotenv.config({path: path.resolve(process.cwd(), '.env')});

import { aiRules } from './ai_rules.js';


export const generateAIResponse = async (userInput) => {
    try {
        const response = await fetch('https://api.openai.com/v1/chat/completions', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${process.env.OPENAI_API_KEY}`
            },
            body: JSON.stringify({
                model: 'gpt-4o-mini',
                messages: [
                    { role: 
                        'system', content: `${aiRules.description} 
                        Rules:${aiRules.rules.map((rule, index) => `${index + 1}. ${rule}`).join('\n')}`
                    },

                    {role: 'user', content: userInput}
                ]
            })
        });

        const responseText = await response.text();

        if (!response.ok) {
            throw new Error(
                `OpenAI API error ${response.status}: ${responseText}`
            );
        }

        const data = JSON.parse(responseText);

        const aiResponse = data.choices[0].message.content;

        console.log('DONIX RESPONSE:', aiResponse);

        return aiResponse;

    } catch (error) {

        console.error('Error generating AI response:', error);

        return null;
    }
};