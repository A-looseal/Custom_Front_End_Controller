
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
StaticJsonDocument<200> doc;
char incomingData[200];

int systemID;
int deviceID;
int desiredState;

SoftwareSerial Serial1(2, 3);

void setup()
{
    Serial.begin(9600);
    Serial.println(F("connected to Serial 0..."));

    Serial1.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);

    analogWrite(9, 0);

    analogWrite(10, 255);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    analogWrite(10, 0);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

    analogWrite(10, 255);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    analogWrite(10, 0);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

    analogWrite(10, 255);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    analogWrite(10, 0);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    // get data
    if (Serial1.available() > 0)
    {
        // store the incoming data
        Serial1.readBytesUntil('\n', incomingData, 200);

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, incomingData);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        // Fetch values.
        //
        // Most of the time, you can rely on the implicit casts.
        // In other case, you can do doc["time"].as<long>();
        systemID = doc["systemID"];
        deviceID = doc["deviceID"];
        desiredState = doc["desiredState"];

        Serial.print(F("systemID: "));
        Serial.println(systemID);
        Serial.print(F("deviceID: "));
        Serial.println(deviceID);
        Serial.print(F("desiredState: "));
        Serial.println(desiredState);
    } // end getting data

    // Serial.println(F("..."));
    // Serial.print(F("systemID: "));
    // Serial.println(systemID);

    // if this device was selected
    if (systemID == 1)
    {
        // put a check here
        Serial.println(F("processed system id "));

        // check which device
        switch (deviceID)
        {
        case 1: // first device connected to the system
                // check desired state

            // check here

            switch (desiredState)
            {
            case 1:
                digitalWrite(LED_BUILTIN, HIGH);
                analogWrite(10, 255);
                // put a check here
                break;

            case 0:
                digitalWrite(LED_BUILTIN, LOW);
                analogWrite(10, 0);
                // put a check here
                break;

            default:
                break;
            }
            break;

            // device start
        case 2: // first device connected to the system
                // check desired state
            switch (desiredState)
            {
            case 1:
                digitalWrite(LED_BUILTIN, HIGH);
                break;
            case 0:
                digitalWrite(LED_BUILTIN, LOW);
                break;
                // device end

                // device start
            case 3: // first device connected to the system
                    // check desired state
                switch (desiredState)
                {
                case 1:
                    digitalWrite(LED_BUILTIN, HIGH);
                    analogWrite(10, 255);   //beep on
                    delay(1000);
                    analogWrite(10, 0);     //beep off
                    delay(1000);
                    analogWrite(10, 255);   //beep on
                    delay(1000);
                    analogWrite(10, 0);     //beep off
                    delay(1000);
                    analogWrite(10, 255);   //beep on
                    break;
                case 0:
                    digitalWrite(LED_BUILTIN, LOW);
                    break;
                    // device end
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        Serial.flush();
    }