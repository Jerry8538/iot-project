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

#define FWSPD 100
#define BWSPD -75

int count = 0;

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

  // simple logic time
  if (digitalRead(SIR)) {
    /*
    mv(FWSPD, BWSPD);
    delay(100);
    */
    mv(FWSPD,FWSPD);
    delay(200);
    mv(0, 0);
    delay(count * 1000);
  }
  line();
}
