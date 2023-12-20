

#include <ctype.h>
#include "Keypad.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET 2         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

/*4*4 KEYPAD STUFF*/
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
// define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte colPins[COLS] = { 6, 7, 8, 9 };      // connect to the column pinouts of the keypad
byte rowPins[ROWS] = { 10, 11, 12, 13 };  // connect to the row pinouts of the keypad
/*END 4*4 KEYPAD STUFF*/

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int cursorPosition_loadingBar_left = 0;     //set the cursor position to the begining
int cursorPosition_loadingBar_right = 120;  //set the cursor position to the begining

//sleep timer
int time_sleep_interval = 100;  // the amount of time before the software goes into sleep phase
ulong time_sleep_previousMillis;
ulong time_sleep_currentMillis;

//input timer
int time_caret_interval = 100;  // the amount of time before the software goes into sleep phase
ulong time_caret_previousMillis;
ulong time_caret_currentMillis;

//loading bar timer
int time_loadingbar_interval = 1;  // the amount of time betwen each loading bar tick
ulong time_loadingbar_previousMillis;
ulong time_loadingbar_currentMillis;

int verticalPosition = 10;
int randomDigits[2];
ulong sleepTimer_Interval = 10;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation successfull"));

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  display.display();
  delay(20);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.clearDisplay();
  delay(200);
  randomDigits[0] = random(0, 9);
  randomDigits[1] = random(0, 9);

  
sleepTimer_Interval = sleepTimer_Interval * 1000 / 120;
}


void loop() {
  ulong currentMillis = millis();  //get current time from cpu

  //this is taking a reading from the keypad once every loop
  char keypadReading = customKeypad.getKey();
  loadingBar(currentMillis, sleepTimer_Interval);

  display.display();
}


void loadingBar(ulong _currentTime, int _IntervalTime) {
  if (_currentTime - time_loadingbar_previousMillis >= _IntervalTime) {  //if 1 second has passed, do the following
    time_loadingbar_previousMillis = _currentTime;  //save last time we checked the time
    //display.setCursor(cursorPosition_loadingBar_left, 0);                        //set the cursor position
    display.drawPixel(cursorPosition_loadingBar_left, 0,SSD1306_WHITE);

    cursorPosition_loadingBar_left = cursorPosition_loadingBar_left + 1;  //update cursor position for next loop

    if (cursorPosition_loadingBar_left >= 120) {
      cursorPosition_loadingBar_left = 0;
      
      generateNumber();
    }

  }
}




void generateNumber() {

  reset();
  display.setCursor(58, 10);
  display.print(randomDigits[0]);
  display.println(randomDigits[1]);
  display.display();
}

void reset(){
  display.clearDisplay();
  randomDigits[0] = random(0, 9);
  randomDigits[1] = random(0, 9);
}