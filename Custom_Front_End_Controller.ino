#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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
char backButton = '*';
char enterButton = '#';
/*END 4*4 KEYPAD STUFF*/

/*OLED SCREEN STUFF*/
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET 2        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
/*END OLED SCREEN STUFF*/

char systemID[2];
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
  // draw display
  display.clearDisplay();
  display.setCursor(15, 15);
  display.print(F("To begin press #"));
  display.display();

  char keypadReading = customKeypad.getKey();
  if (keypadReading == '#')
  {
    Serial.println(F("Key # pressed"));
    display.clearDisplay();
    display.setCursor(15, 15);
    display.print(F("Key # pressed"));
    display.display();
    delay(2000);
  }
  else
  {
    Serial.println(F("No input detected."));
  }

  delay(10); // delay between loops
}

/****CUSTOM FUNCTIONS****/

// get system id
void GetSystemID()
{
  for (size_t i = 0; i < 2;) // start a loop
  {
    display.clearDisplay();
    display.setCursor(10, 0); // Set the cursor position
    display.print(F("Input a System ID."));

    // below we get two digits from the keypad
    char keypadReading = customKeypad.getKey(); // take a polling from the keypad and store it
    // if there is data in the buffer && it is valid
    if (keypadReading > 0 && keypadReading != 'backButton' && keypadReading != 'enterButton' && keypadReading != 'A' && keypadReading != 'B' && keypadReading != 'C' && keypadReading != 'D')
    {
      systemID[i] = keypadReading; // transfer the data from the buffer to the current system array || convert from char to int
      Serial.print("updated system ID: digit: ");
      Serial.println(i);

      // update the screen inside the for loop to see the updates
      display.setCursor(0, 10); // Set the cursor position
      display.print(F("System_ID = "));
      display.print(systemID[0]);   // print first number in the array
      display.println(systemID[1]); // print second number in the array
      display.display();            // render the contents to the screen
      i++;                          // increase the loop counter for the next run
    }                               // end of main input check
  }                                 // end of for loop
  Serial.println("Successfully got a system ID input.");
} // end of function
