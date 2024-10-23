import ollama from 'ollama';
import express from 'express';
import bodyParser from 'body-parser';

const app = express();
app.use(bodyParser.json());

// API route to generate questions and answers
app.post('/generateQuestion', async (req, res) => {
  const body = req.body
  console.log(body);
  const topic = body.topic;
  const difficulty = body.difficulty;

  if (!topic) {
    res.status(400).json({ error: 'Topic is required' });
    return;
  }
  
  try {
    console.log("Generating question...");
    const response = await ollama.chat({
      model: 'llama3.2',
      messages: [{ 
        role: 'system', 
        content: `Imagine yourself as someone that comes up with multiple choice questions that entertains the people in a fun and creative way through a touchscreen device. Generate a ${difficulty} question with 4 options, with 1 being the correct option and 3 being the wrong option about ${topic}. Send the question string which has a key of "question", send an array of 3 incorrect answer which has a key of "incorrectAnswer", and a string of the correct answer which has a key of "correctAnswer". Format it in a pure json format, with your response starting with a "{", do not include anything else in front as it is important for json formatting. Also do not add any form of indexing such as "A) or B) or C)" but make the beginning letter of each option and question capital, DO NOT put the whole answers or question capital, only the beggining of the letter of each option and the question only. At the end of the list, remember to end the JSON with a "}". Remember NOT TO use labelling like "a) or A). Try not to repeat the same questions and think of different creative questions, no matter their difficulty. Make the options within 10 characters. The incorrectAnswer should not contain the correctAnswer. REMEMBER to keep the options within 10 CHARACTERS INCLUDING SPACE while making the option make sense. This is a good example of the format only, the options should have less characters, should only be 10 including spaces and the question should be more creative: {"question": "What Is A Circle","incorrectAnswer": ["Rounded Shape","Oval Figure","Squarish"],"correctAnswer": "Circle"}. Remember to return 1 question, 3 incorrectAnswer and 1 correctAnswer. DO NOT USE THE QUESTION IN THE GIVEN EXAMPLE, ONLY FOLLOW THE FORMAT.  DO NOT COME UP WITH DUMB QUESTIONS. Ask questions that are relevant to ${topic}, DO NOT ask questions where the answer or keywords is inside the question. The correctAnswer should not be in the question itself. The characters in each option should be less than 10. Remember to keep the number of characters in each correctAnswer, incorrectAnswer options to less than 10 characters for small screen. Make the questions make sense and question should not contain correctAnswer else it is a bad question` 
      }],
      format: "json",
      keep_alive: 100,
      options: {
        temperature: 1,
        top_p: 0.8,             // less = focused and conservative, default 0.9
        repeat_penalty: 1.2,
        // repeat_last_n: 25,
      }

    });

    console.log("Raw response:", response.message.content);

    try {
      const result = JSON.parse(response.message.content);
      if (result.question && result.correctAnswer) {
        res.json(result);
      } else {
        throw new Error('Invalid response format');
      }
    } catch (parseError) {
      console.error("Error parsing JSON:", parseError);
      res.status(500).json({ error: 'Error parsing response from model' });
    }
  } catch (apiError) {
    console.error("API call error:", apiError);
    res.status(500).json({ error: 'Error calling the language model API' });
  }
});

// Start the server
const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});

