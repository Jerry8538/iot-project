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
const unsigned long RESET_TIME = 3000; // 3 seconds timeout

void checkSensors() {
  digitalWrite(TRIG_PIN1, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN1, LOW);
  double distance1 = pulseIn(ECHO_PIN1, HIGH) * SOUND_SPD / 2 / 10000;
  // Serial.println(distance1);

  digitalWrite(TRIG_PIN2, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN2, LOW);
  double distance2 = pulseIn(ECHO_PIN2, HIGH) * SOUND_SPD / 2 / 10000;
  // Serial.println(distance2);
  unsigned long current_time = millis();

  // Detect object within threshold
  if (distance1 > 0 && distance1 < THRESHOLD && !sensor1_triggered) {
    sensor1_triggered = true;  // Mark sensor1 as triggered first
    sensor1_time = current_time;
    Serial.println("S1 first");
  }
  if (distance2 > 0 && distance2 < THRESHOLD && sensor1_triggered && !sensor2_triggered) {
    sensor2_triggered = true;  // Mark sensor2 as triggered AFTER sensor1
    count++;
    Serial.println("Incremented Count: " + String(count));
    sensor1_triggered = sensor2_triggered = false;  // Reset flags
  }

  // Reverse detection (sensor2 first, then sensor1)
  if (distance2 > 0 && distance2 < THRESHOLD && !sensor2_triggered) {
    sensor2_triggered = true;
    sensor2_time = current_time;
    Serial.println("S2 first");
  }
  if (distance1 > 0 && distance1 < THRESHOLD && sensor2_triggered && !sensor1_triggered) {
    sensor1_triggered = true;
    count--;
    Serial.println("Decremented Count: " + String(count));
    sensor1_triggered = sensor2_triggered = false;
  }

  // Reset triggers if no second trigger within 3 seconds
  if (sensor1_triggered && (current_time - sensor1_time > RESET_TIME)) {
    sensor1_triggered = false;
  }
  if (sensor2_triggered && (current_time - sensor2_time > RESET_TIME)) {
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
