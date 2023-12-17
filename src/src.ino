#include "Keypad.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <Wire.h>
#include <SPI.h>
/*END INCLUSIONS*/

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
char null = '_';
char key_no = '*';
char key_yes = '#';
/*END 4*4 KEYPAD STUFF*/

/*OLED SCREEN STUFF*/
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET 2        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
/*END OLED SCREEN STUFF*/

const byte stage_verify = -2;
const byte stage_error = -1;
const byte stage_complete = 0;
const byte stage_idle = 1;
const byte stage_getSystemID = 2;
const byte stage_getDeviceID = 3;
const byte stage_getDesiredState = 4;
byte previousStage;
byte currentStage = stage_idle;

char systemID[2] = {null, null};
char deviceID[2] = {null, null};
char desiredState;

/*HOUSE KEEPING*/
// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// initialize an instance of class ADAFRUIT_SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/*END HOUSE KEEPING*/

void setup()
{
  Serial.begin(9600);

  /*BEGIN KEYPAD SETUP*/
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation succedded"));
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  delay(500);
  /*END KEYPAD SETUP*/
}

void loop()
{
  Idle();
  Input_SystemID();
  Input_DeviceID();
  Input_OnOff();
  VerifyStage();
}

// todo : fadf
/* todo : this is */

/****CUSTOM FUNCTIONS****/

void Idle()
{
  if (currentStage == stage_idle)
  {
    // draw display
    display.clearDisplay();
    display.setCursor(15, 15);
    display.print(F("To begin press #"));
    display.display();

    char keypadReading = customKeypad.getKey();
    if (keypadReading == key_yes)
    {
      Serial.println(F("Key # pressed"));

      previousStage = currentStage;     // set the previous stage as the current stage
      currentStage = stage_getSystemID; // go to the next stage
      delay(500);
    }
  }
}

// get system id
void Input_SystemID()
{
  if (currentStage == stage_getSystemID) // if the current stage is on get system id input from the user
  {

    for (size_t _stageCounter = 0; _stageCounter < 3;) // stage 1 loop
    {
      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(0, 0);         // set the cursor position
      display.print(F("System ID: ")); // print to the display
      display.print(systemID[0]);      // select the first index of the system id array
      display.println(systemID[1]);    // select the second index of the system id array

      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        /*TODO: prevent going below 0*/
        _stageCounter--;                // set the stage back to previous
        systemID[_stageCounter] = null; // clear current system id value
        display.clearDisplay();         // clear the display buffer contents
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if (_stageCounter < 2 && keypadReading > 0 && keypadReading != key_no && keypadReading != key_yes && keypadReading != 'A' && keypadReading != 'B' && keypadReading != 'C' && keypadReading != 'D')
      {
        systemID[_stageCounter] = keypadReading; // assign the input from keypad to the system id value
        _stageCounter++;                         // increase stage to next
      }

      // only do the following on confirmation stage
      if (_stageCounter >= 2)
      {
        display.setCursor(20, 25); // set the cursor position
        display.print(F("R U SURE?"));
        // if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          _stageCounter++; // increase the stage
        }
      }

      display.display();               // render everything to the display
    }                                  // end stage 1 loop
    previousStage = stage_getSystemID; // set the previous stage as the current stage
    currentStage = currentStage++;     // go to the next stage
  }                                    // end of function
} // END "INPUT SYSTEM ID" FUNCTION

// get system id
void Input_DeviceID()
{
  if (currentStage == stage_getDeviceID) // if the current stage is on get Device id input from the user
  {

    for (size_t _stageCounter = 0; _stageCounter < 3;) // stage 1 loop
    {
      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(0, 0);         // set the cursor position
      display.print(F("Device ID: ")); // print to the display
      display.print(deviceID[0]);      // select the first index of the Device id array
      display.println(deviceID[1]);    // select the second index of the Device id array

      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        /*TODO: prevent going below 0*/
        _stageCounter--;                // set the stage back to previous
        deviceID[_stageCounter] = null; // clear current system id value
        display.clearDisplay();         // clear the display buffer contents
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if (_stageCounter < 2 && keypadReading > 0 && keypadReading != key_no && keypadReading != key_yes && keypadReading != 'A' && keypadReading != 'B' && keypadReading != 'C' && keypadReading != 'D')
      {
        deviceID[_stageCounter] = keypadReading; // assign the input from keypad to the system id value
        _stageCounter++;                         // increase stage to next
      }

      // only do the following on confirmation stage
      if (_stageCounter >= 2)
      {
        display.setCursor(20, 25); // set the cursor position
        display.print(F("R U SURE?"));
        // if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          _stageCounter++; // increase the stage
        }
      }

      display.display();               // render everything to the display
    }                                  // end stage 1 loop
    previousStage = stage_getDeviceID; // set the previous stage as the current stage
    currentStage = currentStage++;     // go to the next stage
  }                                    // end of function
} // END "INPUT SYSTEM ID" FUNCTION

// get system id
void Input_OnOff()
{
  if (currentStage == stage_getDesiredState) // if the current stage is on get Device id input from the user
  {

    for (size_t _stageCounter = 0; _stageCounter < 2;) // stage 1 loop
    {
      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(0, 0);             // set the cursor position
      display.print(F("Desired state: ")); // print to the display
      if (desiredState == 'A')
      {
        display.print(F("On")); //
      }
      else if (desiredState == 'B')
      {
        display.print(F("Off")); //
      }

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        /*TODO: prevent going below 0*/
        _stageCounter--;                // set the stage back to previous
        deviceID[_stageCounter] = null; // clear current system id value
        display.clearDisplay();         // clear the display buffer contents
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if (_stageCounter < 1)
      {
        if (keypadReading == 'A' || keypadReading == 'B')
        {
          desiredState = keypadReading;
        }

        desiredState = 'A'; // assign the input from keypad to the system id value
        _stageCounter++;    // increase stage to next
      }

      // only do the following on confirmation stage
      if (_stageCounter >= 1)
      {
        display.setCursor(20, 25); // set the cursor position
        display.print(F("R U SURE?"));
        // if the yes key has been pressed
        if (keypadReading == key_yes)
        {
          _stageCounter++; // increase the stage
        }
      }

      display.display();                   // render everything to the display
    }                                      // end stage 1 loop
    previousStage = stage_getDesiredState; // set the previous stage as the current stage
    currentStage = stage_verify;           // go to the next stage
  }                                        // end of function
} // END "INPUT SYSTEM ID" FUNCTION

// might delete
void CompleteStage()
{
  if (currentStage == stage_complete)
  {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print(F("Step complete!"));
    display.display();
    delay(500);
  }

  currentStage = previousStage + 1;
}

// error stage
void ErrorStage()
{
  if (currentStage == stage_error)
  {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print(F("ERROR"));
    display.display();
    delay(500);
  }

  currentStage = previousStage + 1;
}

// verify stage
void VerifyStage()
{
  if (currentStage == stage_verify)
  {

    // todo:
    // display the system id
    display.setCursor(0, 0);
    display.print(F("system id:"));
    display.print(systemID[0]);
    display.println(systemID[1]);
    // display the device id
    display.setCursor(0, 5); //(x,)y
    display.print(F("device id:"));
    display.print(systemID[0]);
    display.println(systemID[1]);
    // display the desired state
    // wait for the confirm key to be pressed
    // if back key is pressed, set state back to 1

    delay(30000);
  }

  currentStage = previousStage + 1;
}