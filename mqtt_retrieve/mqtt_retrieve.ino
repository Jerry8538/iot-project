#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ThingSpeak channel ID
#define CHANNEL_ID 2895376

// WiFi credentials
const char* ssid = "yash";
const char* pswd = "getpwned";

// Server and port details
const char* server = "mqtt3.thingspeak.com";
const int port = 1883;

// MQTT device credentials: TRAM
const char* mqtt_client_id = "Kx4DLTQfBwgnNRcvJQ0vAg8";
const char* mqtt_username = "Kx4DLTQfBwgnNRcvJQ0vAg8";
const char* mqtt_password = "ElE2ekiipOK58P2CXhDFroqs";

WiFiClient client;
PubSubClient mqttclient(client);


#define PWMA 23
#define AIN2 22
#define AIN1 21

#define BIN1 19
#define BIN2 18
#define PWMB 5

#define STBY 17

#define RIR 13
#define LIR 27
#define SIR 35 // detect station

#define FWSPD 110
#define BWSPD -90


const unsigned long POST_INTERVAL = 2000;
unsigned long lastPostTime = 0;

int count = 0;
int send = 0;           // flag to check whether the data must be sent or not
int currstation = 3;    // keeps track of the current station the tram is at

void mv(int lspeed, int rspeed) {
  digitalWrite(STBY, HIGH);
  
  if (lspeed >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, lspeed);
  } else {
    digitalWrite(AIN2, HIGH);
    digitalWrite(AIN1, LOW);
    analogWrite(PWMA, -lspeed);
  }
  
  if (rspeed >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, rspeed);
  } else {
    digitalWrite(BIN2, HIGH);
    digitalWrite(BIN1, LOW);
    analogWrite(PWMB, -rspeed);
  }
}

void line() {
  int l = digitalRead(LIR);
  int r = digitalRead(RIR);
  if (l == LOW && r == LOW) {
    mv(FWSPD, FWSPD);
  } else if (l == HIGH && r == LOW) {
    // too far left, turn right
    mv(BWSPD, FWSPD);
  } else if (l == LOW && r == HIGH) {
    // too far right, turn left
    mv(FWSPD, BWSPD);
  }
}


// Publish messages to a ThingSpeak channel.
int mqttPublish(long pubChannelID, String message) {
  String pubTopic ="channels/" + String(pubChannelID) + "/publish";
  return mqttclient.publish(pubTopic.c_str(), message.c_str());
}

char json[1000];
// Function to handle messages from MQTT subscription
void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length) {
  // Print the details of the message that was received to the serial monitor
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, String((char*)payload));

  if (error) {
    Serial.println("invalid JSON");
  } else {
    count = doc["field1"].as<int>();
  }
}

// Subscribe to ThingSpeak channel for updates.
void mqttSubscribe(long subChannelID){
  String subTopic = "channels/"+String(subChannelID)+"/subscribe";
  mqttclient.subscribe(subTopic.c_str());
}

// Connect to WiFi
void connectWifi() {
  WiFi.begin(ssid, pswd);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

// Connect to MQTT server
void mqttConnect() {
  while (!mqttclient.connected())
  {
    // Connect to the MQTT broker
    if (mqttclient.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
      delay(2000);
      Serial.print("MQTT to ");
      Serial.print(server);
      Serial.print (" at port ");
      Serial.print(port);
      Serial.println(" successful.");
    } else {
      Serial.print("MQTT connection failed, rc = ");
      Serial.println(mqttclient.state());
      Serial.println("Will try again in a few seconds...");
      delay(500);
    }
  }
}

// to be modified; to publish station number to field 4 of the channel
void publishStationNumber() {
  if ((millis() - lastPostTime) >= POST_INTERVAL) {
    // Create the payload (data) to send
    String payload = "field4=" + String(currstation);
    if (send) {
      send = 0;
      // Publish to the MQTT topic
      if (mqttPublish(CHANNEL_ID, payload)) {
        Serial.println("Data sent to ThingSpeak: " + String(currstation));
      } else {
        Serial.println("Error: Failed to publish message");
      }
    }
    lastPostTime = millis();
  }
}

void setup() {
  pinMode(PWMA, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(RIR, INPUT);
  pinMode(LIR, INPUT);
  pinMode(SIR, INPUT);
  pinMode(2, OUTPUT);
  Serial.begin(9600);

  delay(2000);
  connectWifi();

  // Configure the MQTT client
  mqttclient.setServer(server, port); 
  // Set the MQTT message handler function.
  mqttclient.setCallback(mqttSubscriptionCallback);
  // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
  mqttclient.setBufferSize(2048);
}
   

void loop() {
  // Reconnect to WiFi if it gets disconnected.
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  // Connect if MQTT client is not connected and resubscribe to channel updates.
  if (!mqttclient.connected()) {
    mqttConnect(); 
    mqttSubscribe(CHANNEL_ID);
  }

  mqttclient.loop();

  digitalWrite(2, HIGH);

  // simple logic time
  if (digitalRead(SIR)) {
    mv(FWSPD,FWSPD);
    delay(200);
    
    mv(0, 0);
    currstation = (currstation%3)+1;
    send = 1;
    publishStationNumber();
    delay(count * 1000);
=======
>>>>>>> 163b3a905313b764e4570dc3086c3bd5d484921e
  }
  line();
}
