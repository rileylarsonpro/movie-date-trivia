#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

const char *wifiPSK = "";              // And the preshared key (wifi password)
const char *wifiSSID = "Riley iPhone"; // In order for this to work, you MUST specify the SSID for your wifi

//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//
const char *server = "172.20.10.13";
//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//

char *movieTopic = "/movie";
char *newQuestionTopic = "/newQuestion";
char *correctTopic = "/correctAnswer";
char *incorrectTopic = "/incorrectAnswer";
String macAddr = WiFi.macAddress();               // Store arduino MAC address as a string
String host = "arduino-" + macAddr.substring(15); // Create hostname for this device
char *connectTopic = "/connect";
char *disconnectTopic = "/disconnect";

WiFiClient wifiClient;               // Instantiate wifi
PubSubClient mqttClient(wifiClient); // Instantiate mqtt client

boolean newQuestion = false;
#define NEW D2
#define A1 D5
#define A2 D6
#define A3 D7

boolean objectLoaded = false;

JSONVar myObject;


void setup()
{                                       // Perform this part once when first powered on
    Serial.begin(9600);                 // Open a serial connection (for debugging)
    delay(10);                          // Wait 10 milliseconds (1/100th of a second)
    Serial.println("Initializing ..."); // DEBUG CODE

    // ** Connect to WiFi network - Adapted from https://github.com/todddb/example-lab/blob/master/example-lab/example-lab.ino
    Serial.print("Connecting to "); // Display debugging connection info
    Serial.println(wifiSSID);       // Print the configured SSID to the Serial Monitor

    WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect

    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

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
    Serial.println(mqttClient.state());          // Show debug info about MQTT state


    pinMode(NEW, INPUT_PULLUP);  
    pinMode(A1, INPUT_PULLUP);  
    pinMode(A2, INPUT_PULLUP);  
    pinMode(A3, INPUT_PULLUP);  
}

void loop()
{
    mqttClient.loop(); // Check for new MQTT messages
    newQuestion = !digitalRead(NEW);
    if(newQuestion == true){
      Serial.println("button pushed");
       mqttClient.publish(newQuestionTopic, "");   
       delay(2000);
    }
    if(objectLoaded == true){
        if(!digitalRead(A1) == true){
      Serial.println("A1 button pushed");
         
       if((int)myObject["correct"] == (int)(int)myObject["A1"]){
         mqttClient.publish(correctTopic, (const char*)myObject["title"]); 
       }
       else{
         mqttClient.publish(incorrectTopic, (const char*)myObject["title"]); 
       }
       delay(1000);
    }
    if(!digitalRead(A2) == true){
      Serial.println("A2 button pushed");
       //mqttClient.publish(newQuestionTopic, "");  
       if((int)myObject["correct"] == (int)(int)myObject["A2"]){
          mqttClient.publish(correctTopic, (const char*)myObject["title"]); 
       }
       else{
         mqttClient.publish(incorrectTopic, (const char*)myObject["title"]); 
       }
       delay(1000);
    }
    if(!digitalRead(A3) == true){
      Serial.println("A3 button pushed");
       //mqttClient.publish(newQuestionTopic, "");   
       if((int)myObject["correct"] == (int)(int)myObject["A3"]){
          mqttClient.publish(correctTopic, (const char*)myObject["title"]); 
       }
       else{
         mqttClient.publish(incorrectTopic, (const char*)myObject["title"]); 
       }
       delay(1000);
    }
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

            Serial.println((const char*)myObject["title"]);
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
        }

        if (myObject.hasOwnProperty("A2"))
        {
            Serial.print("myObject[\"A2\"] = ");

            Serial.println((int)myObject["A2"]);
        }

        if (myObject.hasOwnProperty("A3"))
        {
            Serial.print("myObject[\"A3\"] = ");

            Serial.println((int)myObject["A3"]);
            objectLoaded = true;
        }
    }
}
