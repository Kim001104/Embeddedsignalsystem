#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

// LED 핀 정의
const int RED_LED = 11;
const int YELLOW_LED = 10;
const int GREEN_LED = 9;
const int SWITCH_PIN1 = 2;  // 긴급 모드 (Emergency)
const int SWITCH_PIN2 = 3;  // 주의 모드 (Caution)
const int SWITCH_PIN3 = 4;  // 깜빡임 모드 (Blink)
const int POTENTIOMETER_PIN = A0; // 가변저항 핀

// 상태 변수
volatile bool emergencyMode = false;
volatile bool cautionMode = false;
volatile bool blinkMode = false;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 300;
bool ledState = false;

int potValue = analogRead(POTENTIOMETER_PIN);  // 0~1023 읽기
int brightness = map(potValue, 0, 1023, 0, 255);  // 0~255 변환

// TaskScheduler 객체 생성
Scheduler runner;

// 기본 동작 Task
void task1();
void task2();
void task3();
void task4();
void task5();
void handleBlink();

// Task 객체 생성
Task t1(6000, TASK_FOREVER, &task1, &runner, false);
Task t2(6000, TASK_FOREVER, &task2, &runner, false);
Task t3(6000, TASK_FOREVER, &task3, &runner, false);
Task t4(6000, TASK_FOREVER, &task4, &runner, false);
Task t5(6000, TASK_FOREVER, &task5, &runner, false);

// 긴급 모드 ISR (스위치 1 - 빨간불 유지)
void emergencyISR() {
    if (digitalRead(SWITCH_PIN1) == LOW) {  // 스위치가 눌렸을 때 (FALLING)
        emergencyMode = true;
        runner.disableAll();  // 모든 Task 일시 정지
        Serial.println("Emergency Mode Enabled");
    } else {  // 스위치에서 손을 뗐을 때 (RISING)
        emergencyMode = false;
        runner.enableAll();  // 기본 동작 재개
        Serial.println("Emergency Mode Disabled");
    }
}

// 주의 모드 ISR (스위치 2 - 모든 LED 끄기)
void cautionISR() {
    if (digitalRead(SWITCH_PIN2) == LOW) {  // 스위치가 눌렸을 때 (FALLING)
        cautionMode = true;
        runner.disableAll();
        Serial.println("Caution Mode Enabled");
    } else {  // 스위치에서 손을 뗐을 때 (RISING)
        cautionMode = false;
        runner.enableAll();
        Serial.println("Caution Mode Disabled");
    }
}

// 깜빡임 모드 ISR (스위치 3 - 모든 LED 깜빡이기)
void blinkISR() {
    if (digitalRead(SWITCH_PIN3) == LOW) {  // 스위치가 눌렸을 때 (FALLING)
        blinkMode = true;
        runner.disableAll();
        Serial.println("Blink Mode Enabled");
    } else {  // 스위치에서 손을 뗐을 때 (RISING)
        blinkMode = false;
        runner.enableAll();
        Serial.println("Blink Mode Disabled");
    }
}

// 깜빡임 처리 (스위치 3)
void handleBlink() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = currentMillis;
        ledState = !ledState;
        digitalWrite(RED_LED, ledState);
        digitalWrite(YELLOW_LED, ledState);
        digitalWrite(GREEN_LED, ledState);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    pinMode(SWITCH_PIN1, INPUT_PULLUP);
    pinMode(SWITCH_PIN2, INPUT_PULLUP);
    pinMode(SWITCH_PIN3, INPUT_PULLUP);
    pinMode(POTENTIOMETER_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN1), emergencyISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN2), cautionISR, CHANGE);
    // ✅ digitalToPCINT()를 사용하여 PCINT 설정
    attachPCINT(digitalPinToPCINT(SWITCH_PIN3), blinkISR, CHANGE);

    // Task 시작
    t1.enableDelayed(1);
    t2.enableDelayed(2000);
    t3.enableDelayed(2500);
    t4.enableDelayed(4500);
    t5.enableDelayed(5500);
}

void loop() {


    if (emergencyMode) {
        // 긴급 모드: 빨간불 유지
        digitalWrite(RED_LED, HIGH);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (cautionMode) {
        // 주의 모드: 모든 LED OFF
        analogWrite(RED_LED, 0);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (blinkMode) {
        // 깜빡임 모드
        handleBlink();
    } 
    else {
        // 기본 신호등 동작
        runner.execute();
    }
}

// 1️⃣ 빨간불 (2초)
void task1() {
    analogWrite(RED_LED, 255);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, 0);
    delay(2000);
    analogWrite(RED_LED, 0);
}

// 2️⃣ 노란불 (0.5초)
void task2() {
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 255);
    analogWrite(GREEN_LED, 0);
    delay(500);
    analogWrite(YELLOW_LED, 0);
}

// 3️⃣ 초록불 (2초)
void task3() {
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, 255);
    delay(2000);
    analogWrite(GREEN_LED, 0);
}

// 4️⃣ 초록불 깜빡임 (1초 동안 3번)
void task4() {
    for (int i = 0; i < 3; i++) {
        analogWrite(GREEN_LED, 0);
        delay(250);
        analogWrite(GREEN_LED, 255);
        delay(250);
    }
    analogWrite(GREEN_LED, 0);
}

// 5️⃣ 노란불 (0.5초)
void task5() {
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 255);
    analogWrite(GREEN_LED, 0);
    delay(500);
    analogWrite(YELLOW_LED, 0);
}