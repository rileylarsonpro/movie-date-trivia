// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of displaying string

//Set the pins to the correct ones for your development shield or breakout board.
//when using the BREAKOUT BOARD only and using these software spi lines to the LCD,
//You can use any free pin to define the pins,
//if you don't need to control the LED pin,you can set it to 3.3Vand set the pin definition to -1.
//for example
//pin usage as follow:
//             CS  A0/DC  RESET  SDA  SCK  LED    VCC     GND    
//Arduino Uno  10   9       8    A2   A1   A3   5V/3.3V   GND
//Arduino Mega 10   9       8    A2   A1   A3   5V/3.3V   GND

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//the SDA pin and SCK pin is defined by the system and can't be modified.
//if you don't need to control the LED pin,you can set it to 3.3V and set the pin definition to -1.
//other pins can be defined by youself,for example
//pin usage as follow:
//             CS  A0/DC  RESET  SDA  SCK  LED    VCC     GND    
//Arduino Uno  10   9       8    11   13   A3   5V/3.3V   GND
//Arduino Mega 10   9       8    51   52   A3   5V/3.3V   GND

//Remember to set the pins to suit your display module!
/********************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

// Libraries I added
#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

// for ESP8266 NodeMCU and Wemos D1 Mini D1
   #define MODEL SSD1283A
   #define CS   D8 
   #define CD   D3                                // marked A0 on the SSD1283A breakout
   #define RST  D4 
   #define LED  -1 

   LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); 

   #define BLACK   0x0000
   #define BLUE    0x001F
   #define RED     0xF800
   #define GREEN   0x07E0
   #define CYAN    0x07FF
   #define MAGENTA 0xF81F
   #define YELLOW  0xFFE0
   #define WHITE   0xFFFF

// BEGIN CUSTOM CODE
const char *wifiPSK = "";              // And the preshared key (wifi password)
const char *wifiSSID = "Riley iPhone"; // In order for this to work, you MUST specify the SSID for your wifi

//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//
const char *server = "172.20.10.13";
//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//

char *movieTopic = "/movie";
char *newQuestionTopic = "/newQuestion";
String macAddr = WiFi.macAddress();               // Store arduino MAC address as a string
String host = "arduino-" + macAddr.substring(15); // Create hostname for this device
char *connectTopic = "/connect";
char *disconnectTopic = "/disconnect";

WiFiClient wifiClient;               // Instantiate wifi
PubSubClient mqttClient(wifiClient); // Instantiate mqtt client

boolean newQuestion = false;
String title = "";
int A1 = 0;
int A2 = 0;
int A3 = 0;

JSONVar myObject;

void setup() 
{
  Serial.begin(9600);                 // Open a serial connection (for debugging)
    delay(10);                          // Wait 10 milliseconds (1/100th of a second)
    Serial.println("Initializing ..."); // DEBUG CODE

    // ** Connect to WiFi network - Adapted from https://github.com/todddb/example-lab/blob/master/example-lab/example-lab.ino
    Serial.print("Connecting to "); // Display debugging connection info
    Serial.println(wifiSSID);       // Print the configured SSID to the Serial Monitor

    WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect

    while (WiFi.status() != WL_CONNECTED)
    {                      // If not connected to wifi
        delay(500);        // Pause
        Serial.print("."); // Print a dot each loop while trying to connect
    }
    Serial.println("");
    Serial.println("WiFi connected"); // Print "connected" message to the Serial Monitor

    // ----- Connect to MQTT -----
    Serial.println("Connecting to MQTT Broker"); // Serial debug
    mqttClient.setServer(server, 1883);          // Set up MQTT Connection Info
    mqttClient.setCallback(callback);            // Function to call when new messages are received
    mqttClient.connect(host.c_str());            // Connect to the broker with unique hostname
    mqttClient.subscribe(movieTopic);            // Subscribe to the data topic on the broker
    mqttClient.subscribe(newQuestionTopic);
    Serial.println(mqttClient.state()); // Show debug info about MQTT state

    mylcd.Init_LCD();
    mylcd.Fill_Screen(BLACK);
}

void loop() 
{
  mqttClient.loop(); // Check for new MQTT messages
    if (newQuestion == true)
    {
        mylcd.Set_Text_Mode(0);

        mylcd.Fill_Screen(0x0000);
        mylcd.Set_Text_colour(MAGENTA);
        mylcd.Set_Text_Back_colour(BLACK);
        mylcd.Set_Text_Size(1);

        // handle text wraping
        if(title.length() < 20){
          mylcd.Print_String(title, 0, 0);
        }
        else if (title.length() < 40){
          mylcd.Print_String(title.substring(0, 20), 0, 0);
          mylcd.Print_String(title.substring(20), 0, 9);
        }
        else if (title.length() < 60) {
          mylcd.Print_String(title.substring(0, 20), 0, 0);
          mylcd.Print_String(title.substring(20, 40), 0, 9);
          mylcd.Print_String(title.substring(40), 0, 18);
        }
        else {
          mylcd.Print_String(title.substring(0, 20), 0, 0);
          mylcd.Print_String(title.substring(20, 40), 0, 9);
          mylcd.Print_String(title.substring(40, 60), 0, 18);
          mylcd.Print_String(title.substring(60), 0, 27);
        }


        mylcd.Set_Text_colour(CYAN);
        mylcd.Set_Text_Size(2);
        mylcd.Print_String("1. " + (String)A1, 0, 40);
        mylcd.Print_String("2. " + (String)A2, 0, 70);
        mylcd.Print_String("3. " + (String)A3, 0, 100);

        newQuestion = false;
    }

}

// The callback function is defined above. New MQTT messages are handed to this function.
void callback(char *topicChar, byte *payload, unsigned int length)
{
    String topic = (String)topicChar; // Convert the char* to a String
    String message = "";              // Convert the byte* payload to String
    // There won't be a null character to terminate, so we manually copy each character
    for (int i = 0; i < length; i++)
    {                                // For each character of the payload
        message += (char)payload[i]; // append to the message string
    }

    Serial.print("Message arrived ["); // Serial Debug
    Serial.print(topic);               //    Print the topic name [in brackets]
    Serial.print("] ");                //
    Serial.println(message);           //    And the message

    if (topic == (String)newQuestionTopic)
    {
         Serial.println("New Question comming");
    }
    if (topic == (String)movieTopic)
    {
        Serial.println("parse");
        Serial.println("=====");

        myObject = JSON.parse(message);

        // JSON.typeof(jsonVar) can be used to get the type of the variable
        if (JSON.typeof(myObject) == "undefined")
        {
            Serial.println("Parsing input failed!");
            return;
        }

        Serial.print("JSON.typeof(myObject) = ");
        Serial.println(JSON.typeof(myObject)); // prints: object

        // myObject.hasOwnProperty(key) checks if the object contains an entry for key
        if (myObject.hasOwnProperty("title"))
        {
            Serial.print("myObject[\"title\"] = ");

            Serial.println((const char *)myObject["title"]);
            title = (String)(const char *)myObject["title"];
            
        }

        if (myObject.hasOwnProperty("correct"))
        {
            Serial.print("myObject[\"correct\"] = ");

            Serial.println((int)myObject["correct"]);
            
        }

        if (myObject.hasOwnProperty("A1"))
        {
            Serial.print("myObject[\"A1\"] = ");

            Serial.println((int)myObject["A1"]);
            A1 = (int)myObject["A1"];
        }

        if (myObject.hasOwnProperty("A2"))
        {
            Serial.print("myObject[\"A2\"] = ");

            Serial.println((int)myObject["A2"]);
            A2 = (int)myObject["A2"];
        }

        if (myObject.hasOwnProperty("A3"))
        {
            Serial.print("myObject[\"A3\"] = ");

            Serial.println((int)myObject["A3"]);
            A3 = (int)myObject["A3"];
        }
        newQuestion = true;
    }
}