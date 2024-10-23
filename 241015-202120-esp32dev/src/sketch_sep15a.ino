#include <FS.h>
#include "Free_Fonts.h"            // Include the header file attached to this sketch

#include <TFT_eSPI.h>              // Hardware-specific library
#include <TFT_eWidget.h>           // Widget library
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
const char* ssid =      "Kieran iPhone";
const char* password =  "kierantaniscool";
bool k = false;
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false
uint16_t t_x, t_y; 
bool pressed;
bool initCategoryButtons, initSubCatButtons, initOptionsButtons, initDifficulty, initRESULTS = false;
#define BUTTON_W 100
#define BUTTON_H 50

#define OPTION_BUTTON_W 150
#define OPTION_BUTTON_H 40
#define NEXT_BUTTON_W 100
#define NEXT_BUTTON_H 50

// Disgusting code ik ik but i had no time and im no genius
String globalCorrectAnswer = "null";
String correctAnswer;
String categorySelected = "Nil";
// int numberOfQuestions = 5;
String difficulty = "medium";
bool questionsDownloaded = false;
JsonArray allQuestions;
const char* JsonCorrectAnswer;
const char* question;
JsonArray incorrectAnswers;

// Create button variables
ButtonWidget btnR = ButtonWidget(&tft);
ButtonWidget btnCat1 = ButtonWidget(&tft);
ButtonWidget btnCat2 = ButtonWidget(&tft);
ButtonWidget btnCat3 = ButtonWidget(&tft);

ButtonWidget sub1Cat1 = ButtonWidget(&tft);
ButtonWidget sub2Cat1 = ButtonWidget(&tft);
ButtonWidget sub3Cat1 = ButtonWidget(&tft);

ButtonWidget sub1Cat2 = ButtonWidget(&tft);
ButtonWidget sub2Cat2 = ButtonWidget(&tft);
ButtonWidget sub3Cat2 = ButtonWidget(&tft);

ButtonWidget sub1Cat3 = ButtonWidget(&tft);
ButtonWidget sub2Cat3 = ButtonWidget(&tft);
ButtonWidget sub3Cat3 = ButtonWidget(&tft);

ButtonWidget easyBtn = ButtonWidget(&tft);
ButtonWidget mediumBtn= ButtonWidget(&tft);
ButtonWidget hardBtn = ButtonWidget(&tft);

ButtonWidget btnOption1 = ButtonWidget(&tft);
ButtonWidget btnOption2 = ButtonWidget(&tft);
ButtonWidget btnOption3 = ButtonWidget(&tft);
ButtonWidget btnOption4 = ButtonWidget(&tft);
String options[4] = {"Option 1", "Option 2", "Option 3", "Option 4"};
ButtonWidget btnNext = ButtonWidget(&tft);
ButtonWidget btnhome = ButtonWidget(&tft);

// ButtonWidget fiveBtn = ButtonWidget(&tft);
// ButtonWidget tenBtn= ButtonWidget(&tft);
// ButtonWidget twentyBtn= ButtonWidget(&tft);

enum ProgramState {START, chooseCategory, chooseSubCategory, selectDifficulty, quiz, RESULTS};
ProgramState currentState = START;

void btnR_pressAction(void) {
  Serial.println("Play");
  currentState = chooseCategory;
}

// Button actions for categories
void btnCat1_pressAction() {
  Serial.println("Biology Pressed");
  categorySelected = "Biology";
  currentState = chooseSubCategory; 
}

void btnCat2_pressAction() {
  Serial.println("Physics Pressed");
  categorySelected = "Physics";
  currentState = chooseSubCategory; 
}

void btnCat3_pressAction() {
  Serial.println("Mathematics Pressed");
  categorySelected = "Mathematics";
  currentState = chooseSubCategory; 
}

void sub1Cat1_pressAction() {
  Serial.println("Fungi Pressed");
  categorySelected = "Fungi";
  currentState = selectDifficulty; 
}

void sub2Cat1_pressAction() {
  Serial.println("Virus Pressed");
  categorySelected = "Virus";
  currentState = selectDifficulty; 
}

void sub3Cat1_pressAction() {
  Serial.println("Antibiotics Pressed");
  categorySelected = "Antibiotics";
  currentState = selectDifficulty; 
}

void sub1Cat2_pressAction() {
  Serial.println("Heat Pressed");
  categorySelected = "Heat";
  currentState = selectDifficulty; 
}

void sub2Cat2_pressAction() {
  Serial.println("Electricity Pressed");
  categorySelected = "Electricity";
  currentState = selectDifficulty; 
}

void sub3Cat2_pressAction() {
  Serial.println("Force Pressed");
  categorySelected = "Force";
  currentState = selectDifficulty; 
}

void sub1Cat3_pressAction() {
  Serial.println("Circles Pressed");
  categorySelected = "Circles";
  currentState = selectDifficulty; 
}

void sub2Cat3_pressAction() {
  Serial.println("Ratio Pressed");
  categorySelected = "Ratio";
  currentState = selectDifficulty; 
}

void sub3Cat3_pressAction() {
  Serial.println("Math Area Pressed");
  categorySelected = "Math Area";
  currentState = selectDifficulty; 
}

void easyBtn_pressAction() {
  Serial.println("Easy Pressed");
  difficulty = "easy";
  currentState = quiz; 
}

void mediumBtn_pressAction() {
  Serial.println("Medium Pressed");
  difficulty = "medium";
  currentState = quiz; 
}

void hardBtn_pressAction() {
  Serial.println("Hard Pressed");
  difficulty = "hard";
  currentState = quiz; 
}

// void fiveBtn_pressAction() {
//   Serial.println("5 Questions Pressed");
//   numberOfQuestions = 5;
//   currentState = quiz; 
// }

// void tenBtn_pressAction() {
//   Serial.println("10 Questions Pressed");
//   numberOfQuestions = 10;
//   currentState = quiz; 
// }

// void twentyBtn_pressAction() {
//   Serial.println("20 Questions Pressed");
//   numberOfQuestions = 20;
//   currentState = quiz; 
// }

void btnOpt1_pressAction() {
  if (options[0] == globalCorrectAnswer) {
    showCorrect();
  } else {
    Serial.println("Incorrect Option 1");
    showWrong();
  }
}

void btnOpt2_pressAction() {
  if (options[1] == globalCorrectAnswer) {
    Serial.println("Correct Option 2");
    showCorrect();
  } else {
    Serial.println("Incorrect Option 2");
    showWrong();
  }
}

void btnOpt3_pressAction() {
  if (options[2] == globalCorrectAnswer) {
    Serial.println("Correct Option 3");
    showCorrect();
  } else {
    Serial.println("Incorrect Option 3");
    showWrong();
  }
}

void btnOpt4_pressAction() {
  if (options[3] == globalCorrectAnswer) {
    Serial.println("Correct Option 4");
    showCorrect();
  } else {
    Serial.println("Incorrect Option 4");
    showWrong();
  }
}

void next_pressAction() {
  Serial.println("NEXT");
  // Reset the options for the next question
  initOptionsButtons = false;
  questionsDownloaded = false;
  currentState = quiz;
}

void home_pressAction(void) {
  Serial.println("home was pressed");
  home();
}

void showCorrect() {
  initRESULTS = false;
  currentState = RESULTS;
  tft.fillScreen(TFT_BLACK);
  tft.fillRoundRect(70, 40, 200, 70, 5, TFT_GREEN);
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(FF24);
  tft.drawString("Correct!", 85, 55);
  tft.setFreeFont(FF18);
}

void showWrong() {
  initRESULTS = false;
  currentState = RESULTS;
  tft.fillScreen(TFT_BLACK);
  tft.fillRoundRect(115, 20, 120, 30, 5, TFT_GOLD);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("Incorrect", 130, 25);
  tft.setTextColor(TFT_WHITE);
  String formattedText = wrapText(globalCorrectAnswer, 20);
  tft.drawString("Correct Answer:", 0, 60);
  drawWrappedText(formattedText, 0, 80);
}

void initButtons() {
  Serial.println("Buttons Initialised!");
  uint16_t x = (tft.width() - BUTTON_W) / 2;
  uint16_t y = tft.height() / 2 - BUTTON_H - 10;

  y = tft.height() / 2 + 40;
  btnR.initButtonUL(110, 160, 100, 50, TFT_WHITE, TFT_BLACK, TFT_GREEN, "Play!", 1);
  Serial.println("Button X: " + x);
  Serial.println("Button Y: " + y);
  Serial.println("Button Width: " + BUTTON_W);
  Serial.println("Button Height: " + BUTTON_H);
  btnR.setPressAction(btnR_pressAction);
  btnR.drawSmoothButton(false, 3, TFT_BLACK);
}

void changeBtnRColor() {
  btnR.drawSmoothButton(true);  
}

void home() {
  initCategoryButtons, initSubCatButtons, initOptionsButtons, initDifficulty, initRESULTS = false;
  currentState = START;
  setup();
}

void initOptionButtons(const String incorrectOptions[3], const String& correctAnswer) {
  // Randomly select an index for the correct answer
  int randomNumber = random(0, 4);

  // Set the correct answer at the random index
  options[randomNumber] = correctAnswer;
  Serial.println("Random Number: " + String(randomNumber));
  
  // Fill the rest of the options with incorrect answers
  int j = 0;
  for (int i = 0; i < 4; i++) {
    if (i != randomNumber) {
      options[i] = incorrectOptions[j];
      Serial.print("Option " + String(i));
      Serial.println(": " + options[i]);
      j++;
    }
  }

  uint16_t x = tft.width() / 2;
  uint16_t y = tft.height() / 2;

  // Convert String to char* and initialize buttons
  btnOption1.initButtonUL(x + 5, y + 10, 152, 45, TFT_WHITE, TFT_GREEN, TFT_BLACK, options[1].c_str(), 1);
  btnOption1.setPressAction(btnOpt1_pressAction);
  btnOption1.drawSmoothButton(false, 3, TFT_BLACK);

  btnOption2.initButtonUL(x - 157, y + 10, 152, 45, TFT_WHITE, TFT_GREEN, TFT_BLACK, options[0].c_str(), 1);
  btnOption2.setPressAction(btnOpt2_pressAction);
  btnOption2.drawSmoothButton(false, 3, TFT_BLACK);

  btnOption3.initButtonUL(x + 5, y + 60, 152, 45, TFT_WHITE, TFT_GREEN, TFT_BLACK, options[3].c_str(), 1);
  btnOption3.setPressAction(btnOpt3_pressAction);
  btnOption3.drawSmoothButton(false, 3, TFT_BLACK);

  btnOption4.initButtonUL(x - 157, y + 60, 152, 45, TFT_WHITE, TFT_GREEN, TFT_BLACK, options[2].c_str(), 1);
  btnOption4.setPressAction(btnOpt4_pressAction);
  btnOption4.drawSmoothButton(false, 3, TFT_BLACK);

  // btnhome.initButtonUL(x + 70, y - 15, 100, 50, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Home", 1);
  // btnhome.setPressAction(home_pressAction);
  // btnhome.drawSmoothButton(false, 2, TFT_BLACK);

  globalCorrectAnswer = correctAnswer;
  Serial.println("GlobalCorrectAnswer: " + globalCorrectAnswer);
  initRESULTS = false;
}

String wrapText(const String &text, int maxChars) {
  String wrappedText = "";
  int start = 0;
  int length = text.length();
  
  while (start < length) {
    int end = start + maxChars;
    
    // Ensure we don't cut a word in the middle
    if (end < length && text[end] != ' ') {
      int spacePos = text.lastIndexOf(' ', end);
      if (spacePos > start) {
        end = spacePos;
      }
    }
    
    // Add the current line to wrappedText
    wrappedText += text.substring(start, end) + "\n";
    start = end + 1;
  }
  
  return wrappedText;
}

void drawWrappedText(const String &text, int x, int y) {
  int lineHeight = 23;  // Adjust this based on your text size
  int currentY = y;
  
  int start = 0;
  int end = text.indexOf('\n');
  tft.setTextColor(TFT_WHITE);
  while (end != -1) {
    String line = text.substring(start, end);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(line, x, currentY);
    currentY += lineHeight;
    start = end + 1;
    end = text.indexOf('\n', start);
  }
  
  // Draw the last line
  String lastLine = text.substring(start);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(lastLine, x, currentY);
}

void processQuestion() {
  Serial.println("Processing");
  if (!initOptionsButtons) {
    if (question) {
      correctAnswer = String(JsonCorrectAnswer);
      Serial.println("IF question was activated");
      String questionStr = "Question: " + String(question);
      Serial.println(questionStr);

      String correctAnswerStr = "Correct Answer: " + String(correctAnswer);
      Serial.println(correctAnswerStr);

      String incorrectAnswersString;
      serializeJson(incorrectAnswers, incorrectAnswersString);
      Serial.println("Incorrect Answers: " + incorrectAnswersString);

      tft.fillScreen(TFT_BLACK);
      String formattedText = wrapText(question, 25);
      drawWrappedText(formattedText, 20, 20);
      String incorrectOptions[3];
      for (int i = 0; i < 3; i++) {
        incorrectOptions[i] = incorrectAnswers[i].as<String>();
      }
      initOptionButtons(incorrectOptions, correctAnswer);
      initOptionsButtons = true;
      questionsDownloaded = true;
    } else {
      Serial.println("Question not properly generated");
      initOptionsButtons = false;
      questionsDownloaded = false;
      delay(2000);
    }
  }
  
  if (pressed) {
    Serial.println("SIGMA");
    Serial.print("X: ");
    Serial.println(t_x);
    Serial.print("Y: ");
    Serial.println(t_y);
    if (btnOption1.contains(t_x, t_y)) {
      btnOption1.press(true);
      btnOption1.pressAction();
    } else if (btnOption2.contains(t_x, t_y)) {
      btnOption2.press(true);
      btnOption2.pressAction();
    } else if (btnOption3.contains(t_x, t_y)) {
      btnOption3.press(true);
      btnOption3.pressAction();
    } else if (btnOption4.contains(t_x, t_y)) {
      btnOption4.press(true);
      btnOption4.pressAction();
    } else if (btnhome.contains(t_x, t_y)) { 
      btnhome.press(true);
      btnhome.pressAction();
    }
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  WiFi.mode(WIFI_STA);
  tft.setFreeFont(FF18);

  // Calibrate the touch screen and retrieve the scaling factors
  // touch_calibrate();
  initButtons();
  WiFi.begin(ssid, password); 
  Serial.println("Connecting to WiFi..");
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF23);
  tft.drawString("Quizzy!", 100, 40);
  tft.setFreeFont(FF18);

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    // Serial
    Serial.print(".");
    if (k) {
      tft.setTextColor(TFT_GOLD);
      tft.drawString("Connecting to WiFi.", 60, 80);
      Serial.println("RED");
      k = false;
    } else {
      tft.setTextColor(TFT_WHITE);
      tft.drawString("Connecting to WiFi.", 60, 80);
      Serial.println("WHITE");
      k = true;
    }
    delay(500);
  }
  tft.fillRect(60, 80, 260, 30, TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Connected to WiFi!", 60, 80);
  changeBtnRColor();
  Serial.println("Connected to the WiFi network");
}

void loop() {
  static uint32_t scanTime = millis();
  t_x = 9999, t_y = 9999; // To store the touch coordinates
  
  // Scan keys every 50ms at most
  if (millis() - scanTime >= 50) {
    pressed = tft.getTouch(&t_x, &t_y);
    scanTime = millis();

    // Handle button presses based on the current state
    switch (currentState) {
      case START: {
        delay(200);
        if (pressed && btnR.contains(t_x, t_y)) {
          Serial.println("Button R got pressed line 137");
          btnR.press(true);
          btnR.pressAction();
          currentState = chooseCategory;
        }
        break;
      }

      case chooseCategory: {
        if (pressed) {
          if (btnCat1.contains(t_x, t_y)) {
            btnCat1.press(true);
            btnCat1.pressAction();
          } else if (btnCat2.contains(t_x, t_y)) {
            btnCat2.press(true);
            btnCat2.pressAction();
          } else if (btnCat3.contains(t_x, t_y)) {
            btnCat3.press(true);
            btnCat3.pressAction();
          } else {
            btnCat1.press(false);
            btnCat2.press(false);
            btnCat3.press(false);
          }
        } else {
          btnCat1.press(false);
          btnCat2.press(false);
          btnCat3.press(false);
        }

        if (!initCategoryButtons) {
          delay(200);
          tft.fillScreen(TFT_BLACK);
          Serial.println("Choose Category");
          uint16_t x = (tft.width() - BUTTON_W) / 2;
          uint16_t catY = tft.height() / 2 - 3 * BUTTON_H / 2 - 20;
          tft.setTextColor(TFT_WHITE);
          tft.setFreeFont(FF22);
          tft.drawString("Choose Category", x - 50, catY - 10);
          tft.setFreeFont(FF18);

          btnCat1.initButtonUL(x - 15, catY + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "Biology", 1);
          btnCat1.setPressAction(btnCat1_pressAction);
          btnCat1.drawSmoothButton(false, 3, TFT_BLACK);

          btnCat2.initButtonUL(x - 15, catY + BUTTON_H + 40, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GOLD, TFT_BLACK, "Physics", 1);
          btnCat2.setPressAction(btnCat2_pressAction);
          btnCat2.drawSmoothButton(false, 3, TFT_BLACK);

          btnCat3.initButtonUL(x - 15, catY + 2 * (BUTTON_H + 10) + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Maths", 1);
          btnCat3.setPressAction(btnCat3_pressAction);
          btnCat3.drawSmoothButton(false, 3, TFT_BLACK);

          initCategoryButtons = true;
        }
        break;
      }
      case chooseSubCategory: {

      if (!initSubCatButtons) {
          tft.fillScreen(TFT_BLACK);
          delay(600);
          Serial.println("Choose Sub Category");
          uint16_t x = (tft.width() - BUTTON_W) / 2;
          uint16_t y = tft.height() / 2 - 3 * BUTTON_H / 2 -20;
          tft.setTextColor(TFT_WHITE);
          tft.setFreeFont(FF22);
          tft.drawString("Choose Sub-Category", x - 70, y - 10);
          tft.setFreeFont(FF18);

          if (categorySelected == "Biology") {
            sub1Cat1.initButtonUL(x - 15, y + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Fungi", 1);
            sub1Cat1.setPressAction(sub1Cat1_pressAction);
            sub1Cat1.drawSmoothButton(false, 3, TFT_BLACK);

            sub2Cat1.initButtonUL(x - 15, y + BUTTON_H + 40, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "Virus", 1);
            sub2Cat1.setPressAction(sub2Cat1_pressAction);
            sub2Cat1.drawSmoothButton(false, 3, TFT_BLACK);

            sub3Cat1.initButtonUL(x - 15, y + 2 * (BUTTON_H + 10) + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GOLD, TFT_BLACK, "Antibiotics", 1);
            sub3Cat1.setPressAction(sub3Cat1_pressAction);
            sub3Cat1.drawSmoothButton(false, 3, TFT_BLACK);
          } else if (categorySelected == "Physics") {
            sub1Cat2.initButtonUL(x - 15, y + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Heat", 1);
            sub1Cat2.setPressAction(sub1Cat2_pressAction);
            sub1Cat2.drawSmoothButton(false, 3, TFT_BLACK);

            sub2Cat2.initButtonUL(x - 15, y + BUTTON_H + 40, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "Energy", 1);
            sub2Cat2.setPressAction(sub2Cat2_pressAction);
            sub2Cat2.drawSmoothButton(false, 3, TFT_BLACK);

            sub3Cat2.initButtonUL(x - 15, y + 2 * (BUTTON_H + 10) + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GOLD, TFT_BLACK, "Force", 1);
            sub3Cat2.setPressAction(sub3Cat2_pressAction);
            sub3Cat2.drawSmoothButton(false, 3, TFT_BLACK);
          } else if(categorySelected == "Mathematics") {
            sub1Cat3.initButtonUL(x - 15, y + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Circles", 1);
            sub1Cat3.setPressAction(sub1Cat3_pressAction);
            sub1Cat3.drawSmoothButton(false, 3, TFT_BLACK);

            sub2Cat3.initButtonUL(x - 15, y + BUTTON_H + 40, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "Ratio", 1);
            sub2Cat3.setPressAction(sub2Cat3_pressAction);
            sub2Cat3.drawSmoothButton(false, 3, TFT_BLACK);

            sub3Cat3.initButtonUL(x - 15, y + 2 * (BUTTON_H + 10) + 30, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GOLD, TFT_BLACK, "Area", 1);
            sub3Cat3.setPressAction(sub3Cat3_pressAction);
            sub3Cat3.drawSmoothButton(false, 3, TFT_BLACK);
          }
          // btnhome.initButtonUL(x + 70, y - 25, 90, 45, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Home", 1);
          // btnhome.setPressAction(home_pressAction);
          // btnhome.drawSmoothButton(false, 2, TFT_BLACK);
          initSubCatButtons = true;
        }
        if (pressed) {
          if (sub1Cat1.contains(t_x, t_y)) {                  // BIOLOGY
            sub1Cat1.press(true);
            sub1Cat1.pressAction();
          } else if (sub2Cat1.contains(t_x, t_y)) {
            sub2Cat1.press(true);
            sub2Cat1.pressAction();
          } else if (sub3Cat1.contains(t_x, t_y)) {
            sub3Cat1.press(true);
            sub3Cat1.pressAction();
          } else if (sub1Cat2.contains(t_x, t_y)) {           // PHYSICS
            sub1Cat2.press(true);
            sub1Cat2.pressAction();
          } else if (sub2Cat2.contains(t_x, t_y)) {
            sub2Cat2.press(true);
            sub2Cat2.pressAction();
          } else if (sub3Cat2.contains(t_x, t_y)) {
            sub3Cat2.press(true);
            sub3Cat2.pressAction();
          } else if (sub1Cat3.contains(t_x, t_y)) {           // MATH
            sub1Cat3.press(true);
            sub1Cat3.pressAction();
          } else if (sub2Cat3.contains(t_x, t_y)) {
            sub2Cat3.press(true);
            sub2Cat3.pressAction();
          } else if (sub3Cat3.contains(t_x, t_y)) {
            sub3Cat3.press(true);
            sub3Cat3.pressAction();
          } else if (btnhome.contains(t_x, t_y)) {
            btnhome.press(true);
            btnhome.pressAction();
          }
        }
      break;
      }
      case selectDifficulty: {
        if (!initDifficulty) {
          uint16_t x = (tft.width() - BUTTON_W) / 2;
          uint16_t y = tft.height() / 2 - 3 * BUTTON_H / 2 -20;
          tft.fillScreen(TFT_BLACK);
          delay(600);
          Serial.println("Choose Difficulty");
          tft.setTextColor(TFT_WHITE);
          tft.setFreeFont(FF22);
          tft.drawString("Choose Difficulty", x - 50, y - 10);
          tft.setFreeFont(FF18);

          easyBtn.initButtonUL(x - 15, y + 20, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "Easy", 1);
          easyBtn.setPressAction(easyBtn_pressAction);
          easyBtn.drawSmoothButton(false, 3, TFT_BLACK);

          mediumBtn.initButtonUL(x - 15, y + 80, BUTTON_W + 60, BUTTON_H, TFT_WHITE, TFT_CYAN, TFT_BLACK, "Medium", 1);
          mediumBtn.setPressAction(mediumBtn_pressAction);
          mediumBtn.drawSmoothButton(false, 3, TFT_BLACK);

          hardBtn.initButtonUL(x - 15, y + 140, BUTTON_W + 60, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Hard", 1);
          hardBtn.setPressAction(hardBtn_pressAction);
          hardBtn.drawSmoothButton(false, 3, TFT_BLACK);

          // btnhome.initButtonUL(x + 70, y - 25, 90, 45, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Home", 1);
          // btnhome.setPressAction(home_pressAction);
          // btnhome.drawSmoothButton(false, 2, TFT_BLACK);
          initDifficulty = true;
        }

        if (pressed) {
          if (easyBtn.contains(t_x, t_y)) {
            easyBtn.press(true);
            easyBtn.pressAction();
          } else if (mediumBtn.contains(t_x, t_y)) {
            mediumBtn.press(true);
            mediumBtn.pressAction();
          } else if (hardBtn.contains(t_x, t_y)) {
            hardBtn.press(true);
            hardBtn.pressAction();
          } else if (btnhome.contains(t_x, t_y)) {
            btnhome.press(true);
            btnhome.pressAction();
          }
        }
        break;
      }
      // case selectNumQuestion: {
      //   if (!initNumQuestion) { // disabled, code jumps straight to quiz
      //     delay(700);
      //     tft.fillScreen(TFT_BLACK);
      //     Serial.println("Select number of questions");
      //     uint16_t x = (tft.width() - BUTTON_W) / 2;
      //     uint16_t y = tft.height() / 2 - 3 * BUTTON_H / 2 -20;
      //     tft.setTextColor(TFT_WHITE);
      //     tft.drawString("Number of Questions", x - 40, y - 10);

      //     fiveBtn.initButtonUL(x, y + 20, BUTTON_W + 30, BUTTON_H, TFT_WHITE, TFT_GREEN, TFT_BLACK, "5", 1);
      //     fiveBtn.setPressAction(fiveBtn_pressAction);
      //     fiveBtn.drawSmoothButton(false, 3, TFT_BLACK);

      //     tenBtn.initButtonUL(x, y + 80, BUTTON_W + 30, BUTTON_H, TFT_WHITE, TFT_CYAN, TFT_BLACK, "10", 1);
      //     tenBtn.setPressAction(tenBtn_pressAction);
      //     tenBtn.drawSmoothButton(false, 3, TFT_BLACK);

      //     twentyBtn.initButtonUL(x, y + 140, BUTTON_W + 30, BUTTON_H, TFT_WHITE, 0x00c8ff, TFT_BLACK, "20", 1);
      //     twentyBtn.setPressAction(twentyBtn_pressAction);
      //     twentyBtn.drawSmoothButton(false, 3, TFT_BLACK);
      //     initNumQuestion = true; 
      //   }

      //   if (pressed) {
      //     if (fiveBtn.contains(t_x, t_y)) {
      //       fiveBtn.press(true);
      //       fiveBtn.pressAction();
      //     } else if (tenBtn.contains(t_x, t_y)) {
      //       tenBtn.press(true);
      //       tenBtn.pressAction();
      //     } else if (twentyBtn.contains(t_x, t_y)) {
      //       twentyBtn.press(true);
      //       twentyBtn.pressAction();
      //     }
      //   }
      //   break;
      // }
      case quiz: {
      if (!questionsDownloaded) {
        Serial.println("Question Download is false");
        tft.fillScreen(TFT_BLACK);
          if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Wifi Connected");
            HTTPClient http;
              Serial.println("HTTP Client");
              http.begin("http://quizzyaiserver.sytes.net:5000/generateQuestion");
              http.addHeader("Content-Type", "application/json");

              tft.setTextColor(TFT_GREEN);
              tft.setFreeFont(FF23);
              tft.drawString("Generating...", 60, 80);
              tft.setFreeFont(FF18);
              int httpResponseCode = http.POST("{\"topic\": \"" + categorySelected + "\", \"numberOfQuestions\": \"" + difficulty + "\"}");
              Serial.println("POSTING");

            if (httpResponseCode > 0) {
              String response = http.getString();
              Serial.println(httpResponseCode);
              Serial.println(response);

              const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 60;
              DynamicJsonDocument doc(capacity);
              DeserializationError error = deserializeJson(doc, response);

              if (!error) {
                if (doc.size() > 0) {
                  if (doc["allQuestions"]) {
                    allQuestions = doc["allQuestions"];
                    int size = sizeof(allQuestions) / sizeof(allQuestions[0]);
                  } else {
                    JsonCorrectAnswer = doc["correctAnswer"];
                    question = doc["question"];
                    incorrectAnswers = doc["incorrectAnswer"];
                  }
                  
                  processQuestion();
                } else {
                  tft.drawString("Something went wrong.", 20, 20);
                  Serial.println("Response array is empty or null");
                  delay(5000);
                }
              } else {
                tft.drawString("someone tampered with my server code", 20, 20);
                Serial.print("JSON deserialization failed: ");
                Serial.println(error.c_str());
              }
            } else if(httpResponseCode == -11) {
              delay(1000);
              break;
            } else {
              tft.drawString("An error occurred", 60, 20);
              Serial.print("Error on sending POST: ");
              Serial.println(httpResponseCode);
              delay(5000);
            }
            http.end();
          } else {
            tft.drawString("WiFi Failed!", 20, 20);
            Serial.println("Error in WiFi connection");
          }
        } else {
          processQuestion();
        }

        break;
      }
      case RESULTS: {
        if (!initRESULTS) {
          delay(500);
          btnNext.initButtonUL(130, 120, 100, 50, TFT_WHITE, TFT_GREEN, TFT_BLACK, "NEXT", 1);
          btnNext.setPressAction(next_pressAction);
          btnNext.drawSmoothButton(false, 2, TFT_BLACK);

          btnhome.initButtonUL(130, 180, 100, 50, TFT_WHITE, 0x00c8ff, TFT_BLACK, "Home", 1);
          btnhome.setPressAction(home_pressAction);
          btnhome.drawSmoothButton(false, 2, TFT_BLACK);

          initRESULTS = true;
        }
        if (pressed) {
          if (btnNext.contains(t_x, t_y)) {
            btnNext.press(true);
            btnNext.pressAction();
          } else if (btnhome.contains(t_x, t_y)) {
            btnhome.press(true);
            btnhome.pressAction();
          }
        }
        break;
      }
    }
  }
}

// Unneccessary 
void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (!LittleFS.begin()) {
    Serial.println("formatting file system");
    LittleFS.format();
    LittleFS.begin();
  }

  if (LittleFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      LittleFS.remove(CALIBRATION_FILE);
    } else {
      File f = LittleFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    tft.setTouch(calData);
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    } 

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    File f = LittleFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
