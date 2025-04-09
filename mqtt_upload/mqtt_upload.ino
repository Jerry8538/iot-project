#include <WiFi.h>
#include <PubSubClient.h>

#define TRIG_PIN1 32
#define ECHO_PIN1 35
#define TRIG_PIN2 19
#define ECHO_PIN2 21
#define SOUND_SPD 343

// ThingSpeak channel ID
#define CHANNEL_ID 2895376

// WiFi credentials
const char* ssid = "sian";
const char* pswd = "bowlingbluegoalden21";

// Server and port details
const char* server = "mqtt3.thingspeak.com";
const int port = 1883;

// MQTT device credentials
const char* mqtt_client_id = "PC8yCQgRBSwwJSU3JA0pFBU";
const char* mqtt_username = "PC8yCQgRBSwwJSU3JA0pFBU";
const char* mqtt_password = "iMQUm+sgQRKLcF+2PzyI7RW0";

WiFiClient client;
PubSubClient mqttclient(client);

const unsigned long POST_INTERVAL = 5000;
unsigned long lastPostTime = 0;

const int THRESHOLD = 30;
volatile int count = 0;
volatile int prevcount = 0;

bool sensor1_triggered = false;
bool sensor2_triggered = false;
unsigned long sensor1_time = 0;
unsigned long sensor2_time = 0;
const unsigned long RESET_TIME = 3000;


// Returns distance read by first ultrasonic sensor
double reads1() {
  digitalWrite(TRIG_PIN1, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN1, LOW);
  return pulseIn(ECHO_PIN1, HIGH) * SOUND_SPD / 2 / 10000;
}

// Returns distance read by second ultrasonic sensor
double reads2() {
  digitalWrite(TRIG_PIN2, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN2, LOW);
  return pulseIn(ECHO_PIN2, HIGH) * SOUND_SPD / 2 / 10000;
}

void checkSensors() {
  double distance1 = reads1();
  double distance2 = reads2();
  unsigned long current_time;

  // sensor1 triggered at current time
  if (distance1 < THRESHOLD && distance2 > THRESHOLD) { // sensor 1 triggered first?
    Serial.println("S1 first");

    // try reading sensor2 for 1 second after current time
    current_time = millis();

    while (millis() - current_time < 1000) {
      distance2 = reads2();
      if (distance2 < THRESHOLD) {
        count++;
        Serial.println("Incremented Count: " + String(count));
        break;
      }
    }
  } else if (distance2 < THRESHOLD && distance1 > THRESHOLD) { // sensor 2 triggered first?
    Serial.println("S2 first");

    current_time = millis();

    while (millis() - current_time < 1000) {
      distance1 = reads1();
      if (distance1 < THRESHOLD) {
        count--;
        Serial.println("Decremented Count: " + String(count));
        break;
      }
    }
  }
}


// Publish messages to a ThingSpeak channel.
int mqttPublish(long pubChannelID, String message) {
  String pubTopic ="channels/" + String(pubChannelID) + "/publish";
  return mqttclient.publish(pubTopic.c_str(), message.c_str());
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


void publishData() {
  if ((millis() - lastPostTime) >= POST_INTERVAL) {
    // Create the payload (data) to send
    String payload = "field1=" + String(count);
    if (prevcount != count) {
      // Publish to the MQTT topic
      if (mqttPublish(CHANNEL_ID, payload)) {
        Serial.println("Data sent to ThingSpeak: " + String(count));
      } else {
        Serial.println("Error: Failed to publish message");
      }
    }
    prevcount = count;
    lastPostTime = millis();
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  delay(2000);
  connectWifi();

  // Configure the MQTT client
  mqttclient.setServer(server, port); 
}


void loop() {
  // Reconnect to WiFi if it gets disconnected
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  // Connect if MQTT client is not connected
  if (!mqttclient.connected()) {
    mqttConnect();
  }

  mqttclient.loop();

  checkSensors();
  publishData();
  // Small delay to avoid excessive pings
  delay(100);
}
