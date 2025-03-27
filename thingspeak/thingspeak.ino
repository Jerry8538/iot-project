#include <WiFi.h>
#include <HTTPClient.h>

#define TRIG_PIN1 5
#define ECHO_PIN1 18
#define TRIG_PIN2 26
#define ECHO_PIN2 27
#define SOUND_SPD 343

const char* ssid = "sian";
const char* pswd = "bowlingbluegoalden21";

const char* apikey = "C6WHIH80VGQ2HHE4";
const char* server = "https://api.thingspeak.com/update";

const unsigned long POST_INTERVAL = 15000;	// sets post interval to 15 seconds
unsigned long lastPostTime = 0;				// to store the last post time in milliseconds

const int THRESHOLD = 30;
volatile int count = 0;

bool sensor1_triggered = false;
bool sensor2_triggered = false;
unsigned long sensor1_time = 0;
unsigned long sensor2_time = 0;
const unsigned long RESET_TIME = 3000;

double reads1() {
  digitalWrite(TRIG_PIN1, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN1, LOW);
  return pulseIn(ECHO_PIN1, HIGH) * SOUND_SPD / 2 / 10000;
}

double reads2() {
  digitalWrite(TRIG_PIN2, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN2, LOW);
  return pulseIn(ECHO_PIN2, HIGH) * SOUND_SPD / 2 / 10000;
}

void publishData() {
  if (millis() - lastPostTime >= POST_INTERVAL) {
    HTTPClient http;
    int data = random(0, 100);
    String url = server + "?apikey=" + apikey + "&field1=" + String(data);
    http.begin(url);

    int httpcode = http.GET();

    if (httpcode == 200) {
      String payload = http.getString();
      if (payload=="0") {
        Serial.println("Data sent to ThingSpeak: " + String(data));
      } else {
				Serial.println("Error: Failed to send data to ThingSpeak.");
			}
    } else {
      Serial.println("Error: HTTP request failed with error code: " + String(httpcode));
    }

    http.end();
    lastPostTime = millis();
  }
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

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  checkSensors();
  publishData();
  delay(100); // Small delay to avoid excessive pings
}


