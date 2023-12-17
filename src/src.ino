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

const byte stage_sendData = -3;
const byte stage_verify = -2;
const byte stage_error = -1;
const byte stage_complete = 0;
const byte stage_idle = 1;
const byte stage_getSystemID = 2;
const byte stage_getDeviceID = 3;
const byte stage_getDesiredState = 4;
byte previousStage;
// set the starting stage here
byte currentStage = stage_idle;

char systemID[2] = {null, null};
char deviceID[2] = {null, null};
String desiredState = "_";

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
  SendData_(systemID, deviceID, desiredState);
}

// todo : fadf
/* todo : this is */

/****CUSTOM FUNCTIONS****/

void Idle()
{
  if (currentStage == stage_idle)
  {
    Serial.println("got to idle stage");

    // draw display
    display.clearDisplay();
    display.setCursor(15, 15);
    display.print(F("To begin press #"));
    display.display();

    for (size_t _stageCounter = 0; _stageCounter < 1;) // stage 1 loop
    {
      char keypadReading = customKeypad.getKey();
      if (keypadReading == key_yes)
      {
        Serial.println(F("Key # pressed"));

        previousStage = stage_idle;       // set the previous stage as the current stage
        currentStage = stage_getSystemID; // go to the next stage
        _stageCounter++;
      }
    }
  }
  Serial.println("got to end of idle stage");
}

// get system id
void Input_SystemID()
{
  Serial.println("got stystem id stage");
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

        if (_stageCounter != 0) // prevent going below 0*/
        {
          _stageCounter--;                // set the stage back to previous
          systemID[_stageCounter] = null; // clear current system id value
          display.clearDisplay();         // clear the display buffer contents
        }
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
    currentStage = stage_getDeviceID;  // go to the next stage
  }                                    // end of function
  Serial.println("got to end of stystem id stage");
} // END "INPUT SYSTEM ID" FUNCTION

// get system id
void Input_DeviceID()
{
  Serial.println("got stystem id stage");

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
        if (_stageCounter != 0) // prevent going below 0*/
        {
          _stageCounter--;                // set the stage back to previous
          deviceID[_stageCounter] = null; // clear current system id value
          display.clearDisplay();         // clear the display buffer contents
        }
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

      display.display();                  // render everything to the display
    }                                     // end stage 1 loop
    previousStage = stage_getDeviceID;    // set the previous stage as the current stage
    currentStage = stage_getDesiredState; // go to the next stage
  }                                       // end of function
  Serial.println("got stystem id stage");
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
      display.println(desiredState);

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

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if (_stageCounter < 1)
      {
        if (keypadReading == 'A' || keypadReading == 'B')
        {
          if (keypadReading == 'A')
          {
            desiredState = "On";
          }
          else if (keypadReading == 'B')
          {
            desiredState = "Off";
          }
          _stageCounter++; // increase stage to next
        }
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
    display.clearDisplay();
    // todo:
    // display the system id
    display.setCursor(0, 0);
    display.print(F("system id:"));
    display.print(systemID[0]);
    display.println(systemID[1]);
    // display the device id
    display.setCursor(0, 10); //(x,)y
    display.print(F("device id:"));
    display.print(deviceID[0]);
    display.println(deviceID[1]);
    // display the desired state
    display.setCursor(0, 20); //(x,)y
    display.print(F("desired state: "));
    display.println(desiredState);
    // wait for the confirm key to be pressed
    // if back key is pressed, set state back to 1

    display.display();
  }

  currentStage = stage_sendData;
}

// get system id
void SendData_(char _systemID[2], char _deviceID[2], String _desiredState)
{
  if (currentStage == stage_sendData) // if the current stage is on get Device id input from the user
  {

    for (size_t _stageCounter = 0; _stageCounter < 1;) // stage 1 loop
    {
      char keypadReading = customKeypad.getKey(); // takes a reading from the keypad

      display.clearDisplay(); // clear the display buffer contents

      display.setCursor(0, 0);          // set the cursor position
      display.println(F("send it?: ")); // print to the display

      /*can do the following on any stage when the back key is pressed*/
      if (keypadReading == key_no)
      {
        ClearData(); // send everything that was input
        // show that it was cancelled
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(F("CANCELLED!"));
        display.display();

        currentStage = stage_idle; // go back to idle stage
        delay(1000);               // delay for 1 second
        _stageCounter++;

      } // end cancel button

      if (keypadReading == key_yes)
      {
        // todo: send everything that was input

        ClearData(); // clear everything that was input
        // show that it was sent
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(F("SENT!"));
        display.display();

        currentStage = stage_idle; // go back to idle
        delay(1000);               // delay for 1 second
        _stageCounter++;
      }
    }
  }
}

/*HELPER FUNCTIONS*/
// clear everything that was input
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