#define TRIG_PIN1 5
#define ECHO_PIN1 18
#define TRIG_PIN2 19
#define ECHO_PIN2 21
#define SOUND_SPD 343

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

void checkSensors() {
  double distance1 = reads1();
  double distance2 = reads2();
  unsigned long current_time;
  // Serial.println(distance1);
  // Serial.println(distance2);

  // sensor1 triggered at current time
  if (distance1 < THRESHOLD) {
    sensor1_triggered = true;  // Mark sensor1 as triggered first
    Serial.println("S1 first");

    // try reading sensor2 for 1 second after current time
    unsigned long current_time = millis();

    while (millis() - current_time < 1000) {
      distance2 = reads2();
      if (distance2 < THRESHOLD) {
        count++;
        Serial.println("Incremented Count: " + String(count));
        break;
      }
    }
    sensor1_triggered = false;  // Reset flags
  } else if (distance2 < THRESHOLD) {
    sensor2_triggered = true;
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
    sensor2_triggered = false;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
}

void loop() {
  checkSensors();
  delay(100); // Small delay to avoid excessive pings
}
