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

const byte stage_idle = 0;
const byte stage_getSystemID = 1;
const byte stage_getDeviceID = 2;
const byte stage_getDesiredState = 3;
const byte stage_Test = 5;
byte previousStage;
byte currentStage = stage_idle;

char systemID[2] = {null, null};
char deviceID[2];
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
  InputSystemID();
  //InputDeviceID();
  TestStage();
}

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
void InputSystemID()
{
  if (currentStage == stage_getSystemID)
  {

    for (size_t i = 0; i < 3;) // stage 1 loop
    {
      display.clearDisplay();
      delay(20);

      Serial.print("on loop #: ");
      Serial.println(i);

      display.setCursor(0, 0);
      display.print(F("System ID: "));
      display.print(systemID[0]);
      display.println(systemID[1]);

      char keypadReading = customKeypad.getKey();

      Serial.println(F("waiting for input"));

      // can do the following on any stage when the back key is pressed
      if (keypadReading == key_no)
      {
        i--;
        systemID[i] = null;
        display.clearDisplay();
        Serial.println(F("back key pressed"));
      }

      // only do the following if we are on stage 1, or 2 and input is a digit.
      if (i < 2 && keypadReading > 0 && keypadReading != key_no && keypadReading != key_yes && keypadReading != 'A' && keypadReading != 'B' && keypadReading != 'C' && keypadReading != 'D')
      {
        systemID[i] = keypadReading;
        i++;
        Serial.println(F("number has been input"));
      }

      // only do the following on confirmation stage
      if (i >= 2)
      {
        display.setCursor(20, 25);
        display.print(F("R U SURE?"));

        if (keypadReading == key_yes)
        {
          i++;
          Serial.println(F("confirm key pressed"));
        }
      }

      display.display();
    } // end stage 1 loop
    Serial.println(F("Successfully got a system ID input."));
    previousStage = stage_getSystemID; // set the previous stage as the current stage
    currentStage = stage_Test;  // go to the next stage
  }                                    // end of function
}

// get device id
void InputDeviceID()
{
  if (currentStage == stage_getDeviceID)
  {
    display.clearDisplay();
    for (size_t i = 0; i < 2;) // stage 1 loop
    {
      display.setCursor(0, 0);
      display.print(F("Device ID: "));
      display.print(deviceID[0]);
      display.println(deviceID[1]);

      char keypadReading = customKeypad.getKey();

      /*CHECK FOR ERASE*/
      if (keypadReading == key_no)
      {
        display.clearDisplay();
        i--;
      }

      if (keypadReading > 0 && keypadReading != key_no && keypadReading != key_yes && keypadReading != 'A' && keypadReading != 'B' && keypadReading != 'C' && keypadReading != 'D')
      {
        deviceID[i] = keypadReading;

        i++;
      }
      display.display();
    } // end stage 1 loop
    Serial.println("Successfully got a device ID input.");

    display.setCursor(120, 25);
    display.println(F("2/3"));

    delay(2000);
    // confirm input here
    previousStage = currentStage;         // set the previous stage as the current stage
    currentStage = stage_getDesiredState; // go to the next stage

  } // end of function
}

void TestStage()
{
  if (currentStage == stage_Test)
  {
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print(F("Step complete!"));
  display.display();
  }
}