#define trig 26
#define echo 27
double ss = 343;

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  double tm = pulseIn(echo, HIGH);
  Serial.print("Distance in cm: ");
  Serial.println(ss*(tm/10000)/2);
  delay(1000);
}
