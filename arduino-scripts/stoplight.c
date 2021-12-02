#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *wifiPSK = ""; // And the preshared key (wifi password)
const char *wifiSSID = "Riley iPhone";   // In order for this to work, you MUST specify the SSID for your wifi

//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//
const char *server = "172.20.10.13";
//**  Connect to MQTT Adapted from Todd Barrett’s Example code **//

char *dataTopic = "/data";
String macAddr = WiFi.macAddress();       // Store arduino MAC address as a string
String host = "arduino-" + macAddr.substring(15) ;  // Create hostname for this device
char *connectTopic = "/connect";
char *disconnectTopic = "/disconnect";

WiFiClient wifiClient;               // Instantiate wifi
PubSubClient mqttClient(wifiClient); // Instantiate mqtt client

int RED = D7; // Sets which GPIO pin we will use for the LEDS
int YELLOW = D6;
int GREEN = D5;

boolean doorState = true;
boolean redState = false; // Tracks state of LEDS
boolean yellowState = false;
boolean greenState = false;

//Helper function to turn all lights off
void allOff()
{
  digitalWrite(GREEN, LOW);
  greenState = false;
  digitalWrite(RED, LOW);
  redState = false;
  digitalWrite(YELLOW, LOW);
  yellowState = false;
  return;
}

void setup()
{                                     // Perform this part once when first powered on
  Serial.begin(9600);                 // Open a serial connection (for debugging)
  delay(10);                          // Wait 10 milliseconds (1/100th of a second)
  Serial.println("Initializing ..."); // DEBUG CODE

  pinMode(RED, OUTPUT);   // Set the LED pin to Output
  digitalWrite(RED, LOW); // Set the pin to LOW (Off)

  pinMode(YELLOW, OUTPUT);   // Set the LED pin to Output
  digitalWrite(YELLOW, LOW); // Set the pin to LOW (Off)

  pinMode(GREEN, OUTPUT);   // Set the LED pin to Output
  digitalWrite(GREEN, LOW); // Set the pin to LOW (Off)

  // ** Connect to WiFi network - Adapted from https://github.com/todddb/example-lab/blob/master/example-lab/example-lab.ino
  Serial.print("Connecting to "); // Display debugging connection info
  Serial.println(wifiSSID);       // Print the configured SSID to the Serial Monitor

  WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect

  while (WiFi.status() != WL_CONNECTED)
  {                    // If not connected to wifi
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
  mqttClient.subscribe(dataTopic);             // Subscribe to the data topic on the broker

  Serial.println(mqttClient.state());          // Show debug info about MQTT state
}

void redOn()
{
  digitalWrite(RED, HIGH); //   then set the RED to high (turn on)
  redState = true;         //   and track the state

  digitalWrite(YELLOW, LOW); //Turn off other lights
  yellowState = false;
  digitalWrite(GREEN, LOW);
  greenState = false;
}

void yellowOn()
{
  digitalWrite(YELLOW, HIGH); //   then set the YELLOW to high (turn on)
  yellowState = true;         //   and track the state

  digitalWrite(RED, LOW); //Turn off other lights
  redState = false;
  digitalWrite(GREEN, LOW);
  greenState = false;
}
void greenOn()
{
  digitalWrite(GREEN, HIGH); //   then set the GREEN to high (turn on)
  greenState = true;         //   and track the state

  digitalWrite(RED, LOW); //Turn off other lights
  redState = false;
  digitalWrite(YELLOW, LOW);
  yellowState = false;
}
void toggleRed()
{
  digitalWrite(RED, !redState); // flip state
  redState = !redState;

  digitalWrite(GREEN, LOW);
  greenState = false;
  digitalWrite(YELLOW, LOW);
  yellowState = false;
}

void loop()
{
  mqttClient.loop(); // Check for new MQTT messages
  delay(100); // This introduces a little pause in each cycle. Probably helps save some power.
}

// The callback function is defined above. New MQTT messages are handed to this function.
void callback(char *topicChar, byte *payload, unsigned int length)
{
  String topic = (String)topicChar; // Convert the char* to a String
  String message = "";              // Convert the byte* payload to String
  // There won't be a null character to terminate, so we manually copy each character
  for (int i = 0; i < length; i++)
  {                              // For each character of the payload
    message += (char)payload[i]; // append to the message string
  }

  Serial.print("Message arrived ["); // Serial Debug
  Serial.print(topic);               //    Print the topic name [in brackets]
  Serial.print("] ");                //
  Serial.println(message);           //    And the message

  if (topic == (String)dataTopic)
  {   
        if (message == "green_on")
        {
          Serial.print("sending green on");
          greenOn();
        }
        else if (message == "yellow_on")
        {
          yellowOn();
        }
        else if (message == "red_on")
        {
          redOn();
        }
        else if (message == "toggle_red")
        {
          toggleRed();
        }
        if (message == "all_off")
        {
            allOff();
        }
  }
}