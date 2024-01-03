
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define CLIENT_DEVICE_RX 2
#define CLIENT_DEVICE_RX 3
// settings for controlling device state. on|off
#define ON_COMMAND 0
#define OFF_COMMAND 1

// buffer to store incoming data from the client
char incomingData[200];

// list of devices connected to the system
#define DEVICE_1 4
#define DEVICE_2 5
#define DEVICE_3 6
#define DEVICE_4 7
#define DEVICE_5 8
#define DEVICE_6 9
#define DEVICE_7 10
#define DEVICE_8 11

// data storage containers
int systemID;
int deviceID;
int desiredState;

// create software serial object for client device connection
SoftwareSerial ClientDevice(CLIENT_DEVICE_RX, CLIENT_DEVICE_RX);
// create json document object
StaticJsonDocument<200> doc;

void setup()
{
    // connect to host computer via UART
    Serial.begin(9600);
    Serial.println(F("connected to host on hardware serial 0."));

    // connect to client device via UART
    ClientDevice.begin(9600);
    ClientDevice.flush();
    Serial.println(F("connected to client on software serial 1."));

    // set mode of the connected devices
    pinMode(DEVICE_1, OUTPUT);
    digitalWrite(DEVICE_1, OFF_COMMAND);
    pinMode(DEVICE_2, OUTPUT);
    digitalWrite(DEVICE_2, OFF_COMMAND);
    pinMode(DEVICE_3, OUTPUT);
    digitalWrite(DEVICE_3, OFF_COMMAND);
    pinMode(DEVICE_4, OUTPUT);
    digitalWrite(DEVICE_4, OFF_COMMAND);
    // set mode of the led
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    // get data
    if (ClientDevice.available() > 0)
    {
        // store the incoming data
        ClientDevice.readBytesUntil('\n', incomingData, 200);

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
        // check which device
        switch (deviceID)
        {
        case 1: // device 1
                //  check desired state
            switch (desiredState)
            {
                // turn device on
            case 1:
                digitalWrite(DEVICE_1, ON_COMMAND);
                Led_Confirm();
                break;
                // turn device off
            case 0:
                digitalWrite(DEVICE_1, OFF_COMMAND);
                Led_Confirm();
                break;

            default:
                break;
            }
            break;

        case 2: // device 2
                // check desired state
            switch (desiredState)
            {
            case 1:
                digitalWrite(DEVICE_2, ON_COMMAND);
                Led_Confirm();
                break;

            case 0:
                digitalWrite(DEVICE_2, OFF_COMMAND);
                Led_Confirm();
                break;

            default:
                break;
            }

        case 3: // device 3
                // check desired state
            switch (desiredState)
            {
            case 1:
                digitalWrite(DEVICE_3, ON_COMMAND);
                Led_Confirm();
                break;
            case 0:
                digitalWrite(DEVICE_3, OFF_COMMAND);
                Led_Confirm();
                break;
                // device end
            default:
                break;
            }

        case 4: // device 4
                // check desired state
            switch (desiredState)
            {
            case 1:
                digitalWrite(DEVICE_4, ON_COMMAND);
                Led_Confirm();
                break;
            case 0:
                digitalWrite(DEVICE_4, OFF_COMMAND);
                Led_Confirm();
                break;
                // device end
            default:
                break;
            }
        default:
            break;
        }
        // clear the serial buffer
        ClientDevice.flush();
        *incomingData = 0;
        // clear data containers
        systemID = -1;
        deviceID = -1;
        desiredState = -1;
    }
}

void Led_Confirm()
{
    digitalWrite(LED_BUILTIN, ON_COMMAND);
    delay(250);
    digitalWrite(LED_BUILTIN, OFF_COMMAND);
}