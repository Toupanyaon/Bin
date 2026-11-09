#include <Servo.h>

// ขากำหนด
#define TRIG_PIN 9
#define ECHO_PIN 10
#define PIEZO_PIN 8
#define SERVO_PIN 4
#define RAIN_SENSOR_DIGITAL_PIN 7

Servo myServo;

// โน้ตเพลง
#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_G5  784
#define NOTE_C6  1047

// เพลงเปิด
int openMelody[] = {NOTE_G5, NOTE_C6};
int openNoteDurations[] = {12, 8};

// เพลงปิด
int closeMelody[] = {NOTE_C6, NOTE_G5};
int closeNoteDurations[] = {12, 8};

// ฟังก์ชันเล่นเพลง
void playMelody(int *melody, int *noteDurations, int notesLength) {
  for (int thisNote = 0; thisNote < notesLength; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIEZO_PIN, melody[thisNote], noteDuration);
    delay(noteDuration * 1.30);
    noTone(PIEZO_PIN);
  }
}

#define playOpenMelody() playMelody(openMelody, openNoteDurations, 2)
#define playCloseMelody() playMelody(closeMelody, closeNoteDurations, 2)

// เพลงเตือนฝน
void playRainWarning() {
  tone(PIEZO_PIN, NOTE_G4, 300);
  delay(400);
  tone(PIEZO_PIN, NOTE_C5, 300);
  delay(400);
  noTone(PIEZO_PIN);
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // เริ่มต้นที่มุม 0 องศา
  Serial.begin(9600);
}

void loop() {
  // ตรวจจับฝนจากขา Digital
  int rainDetected = digitalRead(RAIN_SENSOR_DIGITAL_PIN); // LOW = ฝนตก
  if (rainDetected == LOW) {
    Serial.println("ฝนตก: งดทำงาน");
    playRainWarning();
    delay(1000);
    return; // ข้ามการทำงาน servo
  }

  // ตรวจจับระยะ Ultrasonic
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("ระยะ: ");
  Serial.print(distance);
  Serial.println(" cm");

  // ถ้าเข้าใกล้ ≤ 15 cm
  if (distance <= 15.0) {
    playOpenMelody();
    myServo.write(90); // เปิด
    delay(2000);       // ค้างไว้ 2 วินาที
    playCloseMelody();
    myServo.write(0);  // ปิด
  }

  delay(500);
}
