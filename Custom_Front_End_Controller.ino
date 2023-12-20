
#include "Keypad.h"
#include <ctype.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <Wire.h>
#include <SPI.h>
/*END INCLUSIONS*/

#define CURRENT_VERSION 1.2

/*4*4 KEYPAD STUFF*/
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
// define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte colPins[COLS] = {6, 7, 8, 9};     // connect to the column pinouts of the keypad
byte rowPins[ROWS] = {10, 11, 12, 13}; // connect to the row pinouts of the keypad
// the folowing is a legend of the kaypad buttons
char null = '_';
char key_no = '*';
char key_yes = '#';
char key_on = 'C';
char key_off = 'D';
/*END 4*4 KEYPAD STUFF*/

/*OLED SCREEN STUFF*/
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET 2        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
/*END OLED SCREEN STUFF*/

#define LOADING_TIME 1                 //
#define LAODING_BAR_START_POSITION 100 //

byte internalStageCounter;
const byte stage_sendData = -3;              // the state # for send data state
const byte stage_verify = -2;                // the index # for verify input state
const byte stage_error = -1;                 // the index # for error state
const byte stage_idle = 1;                   // the index # for idle state
const byte stage_getSystemID = 2;            // the state # for get systemID state
const byte stage_length_getSystemID = 3;     // the amount of key presses we are expecting during this stage. digit[0] > digit[1] > confirm
const byte stage_getDeviceID = 3;            // the state # for get device id state
const byte stage_length_getDeviceID = 3;     // the amount of key presses we are expecting during this stage. digit[0] > digit[1] > confirm
const byte stage_getDesiredState = 4;        // the index # for get desired device state
const byte stage_length_getDesiredState = 2; // the amount of key presses we are expecting during this stage. digit[0] > digit[1] > confirm
byte previousStage;                          // store the previous stage we were on
byte currentStage = stage_idle;              // stores the current stage we are on

// DATA STRUCTURE
char systemID[2] = {null, null};
char deviceID[2] = {null, null};
String desiredState = "_";

/*TIME TRACKING*/
#define DELAY_STARTUP 1000
#define TIMEOUT_SLEEP 10000
#define TIMEOUT_INPUT 10000
ulong previousTime;

/*HOUSE KEEPING*/
// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// initialize an instance of class ADAFRUIT_SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/*END HOUSE KEEPING*/

// common strings
String rusure = "Are you sure?";

void setup()
{
  Serial.begin(9600);
  // KEYPAD SETUP
  customKeypad.setHoldTime(500);

  /*BEGIN SCREEN SETUP*/
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation succedded"));

  /*clear the screen and show logo for 2 seconds*/
  display.clearDisplay();              // Clear the buffer
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
                                       // loop here. if index is odd dont show heart. if index is even show heart.

  DisplayText_Centered(F("BOOTING UP"), 10);
  LoadingSeq(100, 10);
  // todo:
  // dispaly the total startup time
  ulong currentTime = millis();
  display.setCursor(0, 55);
  display.print(currentTime); // print the current time
  display.println(F("ms"));   // print ms
  // todo:
  // display current program version
  display.setCursor(100, 55);
  display.print(F("v"));
  display.println(CURRENT_VERSION); // print the current time
  display.display();                // render the buffer contents to the display
  delay(DELAY_STARTUP);
  /*END SCREEN SETUP*/
  currentStage = stage_idle;
}

void loop()
{
  State_Idle();
  State_GetInput_SystemID();
  State_GetInput_DeviceID();
  State_GetInput_OnOff();
  State_VerifyUserInput();
  State_SendData(systemID, deviceID, desiredState);
}

// ####################################################################################################################
// ####################################################################################################################

/****GENERAL FUNCTIONS****/

void State_Idle()
{
  if (currentStage == stage_idle)
  {
    internalStageCounter = 0;
    for (size_t _stageCounter = 0; _stageCounter < 1;) // stage 1 loop
    {
      display.clearDisplay();
      DisplayText_Centered("To begin press #", 30);
      display.display();

      char keypadReading = customKeypad.getKey();

      // proceed if # key pressed
      if (keypadReading == key_yes)
      {
        previousStage = stage_idle;       // set the previous stage as the current stage
        currentStage = stage_getSystemID; // go to the next stage
        internalStageCounter++;
        _stageCounter++;
      }
    }
  }
}

// ####################################################################################################################
// ####################################################################################################################

/* USER INPUT FUNCTIONS
 * These functions gather specific data from the user.
 * Currently we get (SystemID:XX > DeviceID:XX > DesiredState:X)
 */
// get system id, can be digit 0-9
void State_GetInput_SystemID()
{
  if (currentStage == stage_getSystemID) // if the current stage is on get system id input from the user
  {
    const uint8_t _currentStageLength = stage_length_getSystemID; // determines how many inputs to get from user during this stage

    for (size_t _stageCounter = 0; _stageCounter < _currentStageLength;) //
    {
      display.clearDisplay(); // clear the display buffer contents
      ScreenDesign_PhaseCheckpoint(_stageCounter, _currentStageLength);
      ScreenDesign_StageCheckpoint();

      DisplayText_Centered(F("Enter system ID"), 10);

      char textBuffer[4];
      sprintf(textBuffer, ">%c%c", systemID[0], systemID[1]);
      DisplayText_Centered(textBuffer, 20);

      // display.setCursor(50, 20);  // set the cursor position
      // display.print(F(">"));      // print to the display
      // display.print(systemID[0]); // select the first index of the system id array
      // display.print(systemID[1]); // select the second index of the system id array
      //  display.println(F("<"));                                // print to the display

      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        if (_stageCounter != 0) // prevent going below 0*/
        {
          _stageCounter--;                // set the stage back to previous
          systemID[_stageCounter] = null; // clear current system id value
          // display.clearDisplay();         // clear the display buffer contents
        }
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if ((_stageCounter < _currentStageLength - 1) && (keypadReading > 0) && (keypadReading != key_no) && (keypadReading != key_yes) && (keypadReading != 'A') && (keypadReading != 'B') && (keypadReading != 'C') && (keypadReading != 'D'))
      {
        systemID[_stageCounter] = keypadReading; // assign the input from keypad to the system id value
        _stageCounter++;                         // increase stage to next
      }

      // only do the following on confirmation stage
      if (_stageCounter == _currentStageLength - 1)
      {

        DisplayText_Centered(F("Are you sure?"), 30);

        DisplayText_Centered(F("BACK  |  NEXT"), 55);

        // if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          internalStageCounter++;
          _stageCounter++; // increase the stage
        }
      }

      display.display();               // render everything to the display
    }                                  // end stage 1 loop
    previousStage = stage_getSystemID; // set the previous stage as the current stage
    currentStage = stage_getDeviceID;  // go to the next stage
  }                                    // end of stage
} // END "INPUT SYSTEM ID" FUNCTION

// get device id, can be digit 0-9
void State_GetInput_DeviceID()
{
  if (currentStage == stage_getDeviceID) // if the current stage is on get Device id input from the user
  {
    const uint8_t _currentStageLength = 3;                               // determines how many inputs to get from user during this stage
    for (size_t _stageCounter = 0; _stageCounter < _currentStageLength;) // stage 1 loop
    {
      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(20, 0);              // set the cursor position
      display.println(F("ENTER DEVICE ID")); // print to the displaydisplay.setCursor(45 - fontOffset_x, 10 + fontOffset_y); // set the cursor position
      display.setCursor(45, 10);
      display.print(F(">"));      // print to the display
      display.print(deviceID[0]); // select the first index of the system id array
      display.print(deviceID[1]); // select the second index of the system id array

      ScreenDesign_PhaseCheckpoint(_stageCounter, _currentStageLength);
      ScreenDesign_StageCheckpoint();

      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        if (_stageCounter != 0) // prevent going below 0*/
        {
          _stageCounter--;                // set the stage back to previous
          deviceID[_stageCounter] = null; // clear current system id value
          display.clearDisplay();         // clear the display buffer contents
        }
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if ((_stageCounter < _currentStageLength - 1) && (keypadReading > 0) && (keypadReading != key_no) && (keypadReading != key_yes) && (keypadReading != 'A') && (keypadReading != 'B') && (keypadReading != 'C') && (keypadReading != 'D'))
      {
        deviceID[_stageCounter] = keypadReading; // assign the input from keypad to the system id value
        _stageCounter++;                         // increase stage to next
      }

      // only do the following on confirmation stage
      if (_stageCounter >= _currentStageLength - 1)
      {
        display.setCursor(35, 25); // set the cursor position
        display.print(F("R U SURE?"));
        // if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          internalStageCounter++;
          _stageCounter++; // increase the stage
        }
      }

      display.display();                  // render everything to the display
    }                                     // end stage 1 loop
    previousStage = stage_getDeviceID;    // set the previous stage as the current stage
    currentStage = stage_getDesiredState; // go to the next stage
  }                                       // end of function
} // END "INPUT SYSTEM ID" FUNCTION

// get desire device state. can be on, or off
void State_GetInput_OnOff()
{
  if (currentStage == stage_getDesiredState) // if the current stage is on get Device id input from the user
  {
    const uint8_t _currentStageLength = 2;                               // determines how many inputs to get from user during this stage
    for (size_t _stageCounter = 0; _stageCounter < _currentStageLength;) // stage 1 loop
    {
      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(25, 0);                    // set the cursor position
      display.println(F("Turn deice on or off?")); // print to the

      display.setCursor(45, 10);   // set the cursor position
      display.print(F("< "));      // print to the display
      display.print(desiredState); // select the first index of the system id array
      display.println(F(" >"));    // print to the display

      ScreenDesign_PhaseCheckpoint(_stageCounter, _currentStageLength);
      ScreenDesign_StageCheckpoint();

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        if (_stageCounter != 0) // prevent going below 0*/
        {
          _stageCounter--;        // set the stage back to previous
          desiredState = "_";     // clear current system id value
          display.clearDisplay(); // clear the display buffer contents
        }
      }

      /*only runs if we are expecting a digit from the user */
      if (_stageCounter < _currentStageLength - 1)
      {
        /*only runs if a key was pressed on the keypad*/
        if (keypadReading == key_on || keypadReading == key_off) // only proceed if the keypad reading is A or B
        {
          if (keypadReading == key_on) // only proceed if the keypad reading is A
          {
            desiredState = "ON"; // set the desired state to ON
          }
          else if (keypadReading == key_off) // only proceed if the keypad reading is b
          {
            desiredState = "OFF"; // set the desired state to OFF
          }
          _stageCounter++; // increase stage to next
        }
      }

      // only runs when confirming user input
      if (_stageCounter >= _currentStageLength - 1)
      {
        display.setCursor(35, 25); // set the cursor position
        display.print(F("R U SURE?"));
        // only runs if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          internalStageCounter++;
          _stageCounter++; // increase the stage
        }
      }

      display.display();                   // render everything to the display
    }                                      // end stage 1 loop
    previousStage = stage_getDesiredState; // set the previous stage as the current stage
    currentStage = stage_verify;           // go to the next stage
  }                                        // end of function
} // END "INPUT SYSTEM ID" FUNCTION

// error stage
void State_Error()
{
  if (currentStage == stage_error)
  {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print(F("ERROR"));
    display.display();
    delay(500);
  }
  // internalStageCounter--;
  currentStage = previousStage + 1;
}

// verify stage
void State_VerifyUserInput()
{
  if (currentStage == stage_verify)
  {
    for (size_t i = 0; i < 1; i++)
    {
      char keypadReading = customKeypad.getKey(); // get a reading from the keyboard

      display.clearDisplay();
      // display the system id
      display.setCursor(2, 0);
      display.print(F("SYS_ID:"));
      display.print(systemID[0]);
      display.print(systemID[1]);

      display.print(F(" | "));
      // display the device id
      display.print(F("DEV_ID:"));
      display.print(deviceID[0]);
      display.println(deviceID[1]);
      // display the desired state
      display.setCursor(15, 10); //(x,)y
      display.print(F("DESIRED STATE: "));
      display.println(desiredState);
      // wait for the confirm key to be pressed
      // if back key is pressed, set state back to 1

      ScreenDesign_StageCheckpoint();

      display.setCursor(35, 25); // set the cursor position
      display.print(F("R U SURE?"));
      // if the yes key has been pressed
      if (keypadReading == key_yes)
      {
        display.clearDisplay();
        display.setCursor(20, 10);
        display.print(F("PROCESSING DATA"));
        // loading
        LoadingSeq(50, LOADING_TIME * 30);

        display.display();

        currentStage = stage_sendData;
        internalStageCounter++;
      }
      if (keypadReading == key_no)
      {
        ClearData();
        display.clearDisplay();
        display.setCursor(15, 10);
        display.print(F("CLEARING DATA"));
        // loading here
        LoadingSeq(110, LOADING_TIME);
        currentStage = stage_idle;
        internalStageCounter++;
      }

      display.display();
    }
  }
}

// ####################################################################################################################
// ####################################################################################################################

/* DATA COMMUNICATION FUNCTIONS
 * These functions send and recieve data with the connected systems
 */
// sends the processed data to all connected systems
void State_SendData(char _systemID[2], char _deviceID[2], String _desiredState)
{
  if (currentStage == stage_sendData) // if the current stage is set to send data stage
  {
    const uint8_t _currentStageLength = 1;                               // determines how many inputs to get from user during this stage
    for (size_t _stageCounter = 0; _stageCounter < _currentStageLength;) // stage 1 loop
    {
      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(40, 10);      // set the cursor position
      display.println(F("SEND IT?")); // print to the display
      display.display();

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        ClearData(); // clear everything that was previously input
        // show that it was cancelled
        display.clearDisplay();
        display.setCursor(30, 10);
        display.println(F("CANCELLED!"));
        display.display();

        delay(1000);               // delay for 1 second
        currentStage = stage_idle; // go back to idle stage
        _stageCounter++;

      } // end cancel button

      if (keypadReading == key_yes)
      {
        // todo: send everything that was input

        ClearData(); // clear everything that was input
        // show that it was sent
        display.clearDisplay();
        display.setCursor(20, 10);
        display.println(F("SENDING DATA"));
        LoadingSeq(100, LOADING_TIME);
        display.clearDisplay();
        display.setCursor(50, 10);
        display.println(F("SENT!"));
        display.display();

        currentStage = stage_idle; // go back to idle
        delay(1000);               // delay for 1 second
        _stageCounter++;
      }
      display.display();
    }
  }
}

// ####################################################################################################################
// ####################################################################################################################

/*HELPER FUNCTIONS*/
// clear everything that was previously input by the user
/* finds the horizontal center of the desired text
 *arguments: desired text, y coordinate.*/
void DisplayText_Centered(String _text, int _y)
{
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(_text, 0, 0, &x1, &y1, &width, &height); // get the length of the text

  display.setCursor((SCREEN_WIDTH - width) / 2, _y); // print centered text
  display.println(_text);                            // text to display
}
//
void ClearData()
{
  for (size_t i = 0; i < 2; i++)
  {
    systemID[i] = null;
  }
  for (size_t i = 0; i < 2; i++)
  {
    deviceID[i] = null;
  }
  desiredState = "_";
}
//
void LoadingSeq(uint8_t _startPosition, uint8_t _time)
{
  uint8_t randomSeed = random(_startPosition, SCREEN_WIDTH);

  for (uint8_t i = 0; i < randomSeed; i++)
  {
    display.setCursor(i, 20);
    display.print(F("."));
  }

  for (uint8_t i = randomSeed; i < SCREEN_WIDTH - 5; i++)
  {
    display.setCursor(i, 20);
    display.print(F("."));
    display.display();
    delay(random(_time));
  }
}

/*TIME FUNCTIONS
 * These functions help with time tracking, primarily for non blocking code.
 * also used to track user inout*/
// calculate if the timer has expired
bool IsTimeoutExpired(ulong _interval)
{
  ulong currentTime = millis(); //get the current time from the timer

}

/*DISPLAY FUNCTIONS*/
void ScreenDesign_PhaseCheckpoint(char _stage, char _length)
{
  // if stage is at 0 draw 3 dots
  if (_stage == _length - 3)
  {
    // draw 3 dots
    display.drawRect(0, 60, 2, 2, SSD1306_WHITE);
    display.drawRect(0, 60 - 3, 2, 2, SSD1306_WHITE);
    display.drawRect(0, 60 - 3 - 3, 2, 2, SSD1306_WHITE);
  }

  // if stage is at 1 draw 2 dots
  if (_stage == _length - 2)
  {
    // draw 3 dots
    display.drawRect(0, 60, 2, 2, SSD1306_WHITE);
    display.drawRect(0, 60 - 3, 2, 2, SSD1306_WHITE);
  }

  // todo: if there is only one stage then come here

  // if stage is at 2 draw 1 dots
  if (_stage == _length - 1)
  {
    // draw 3 dots
    display.drawRect(0, 60, 2, 2, SSD1306_WHITE);
  }
}

void ScreenDesign_StageCheckpoint()
{
  switch (internalStageCounter)
  {
  case 1:
    display.drawRect(display.width() - 2, 60, 2, 2, SSD1306_WHITE);             // send it
    display.drawRect(display.width() - 2, 60 - 3, 2, 2, SSD1306_WHITE);         // desired state
    display.drawRect(display.width() - 2, 60 - 3 - 3, 2, 2, SSD1306_WHITE);     // device id
    display.drawRect(display.width() - 2, 60 - 3 - 3 - 3, 2, 2, SSD1306_WHITE); // system id
    break;
  case 2:
    display.drawRect(display.width() - 2, 60, 2, 2, SSD1306_WHITE);         // send it
    display.drawRect(display.width() - 2, 60 - 3, 2, 2, SSD1306_WHITE);     // desired state
    display.drawRect(display.width() - 2, 60 - 3 - 3, 2, 2, SSD1306_WHITE); // device id
    // display.drawRect(display.width() - 2, 60 - 3 - 3 - 3, 2, 2, SSD1306_WHITE); //system id
    break;
  case 3:
    display.drawRect(display.width() - 2, 60, 2, 2, SSD1306_WHITE);     // send it
    display.drawRect(display.width() - 2, 60 - 3, 2, 2, SSD1306_WHITE); // desired state
    // display.drawRect(display.width() - 2, 60 - 3 - 3, 2, 2, SSD1306_WHITE); //device id
    // display.drawRect(display.width() - 2, 60 - 3 - 3 - 3, 2, 2, SSD1306_WHITE); //system id
    break;
  case 4:
    display.drawRect(display.width() - 2, 60, 2, 2, SSD1306_WHITE); // send it
    // display.drawRect(display.width() - 2, 60 - 3, 2, 2, SSD1306_WHITE); //desired state
    // display.drawRect(display.width() - 2, 60 - 3 - 3, 2, 2, SSD1306_WHITE); //device id
    // display.drawRect(display.width() - 2, 60 - 3 - 3 - 3, 2, 2, SSD1306_WHITE); //system id
    break;

  default:
    break;
  }
}
