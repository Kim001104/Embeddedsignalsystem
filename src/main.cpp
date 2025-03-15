#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

// LED 핀 정의
const int RED_LED = 11;
const int YELLOW_LED = 10;
const int GREEN_LED = 9;
const int SWITCH_PIN1 = 2;
const int SWITCH_PIN2 = 3;
const int SWITCH_PIN3 = 4;
const int POTENTIOMETER_PIN = A0;



volatile bool emergencyMode = false;
volatile bool cautionMode = false;
volatile bool blinkMode = false;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 300;
bool ledState = false;

// 전역 변수 선언
int portValue = 0;
int brightness = 0;

// TaskScheduler 객체 생성
Scheduler runner;

// Task 함수 선언
void task1();
void task2();
void task3();
void task4();
void task5();

// Task 객체 생성 (초기에는 비활성화 상태)
Task t1(2000, TASK_FOREVER, &task1, &runner, false);
Task t2(500, TASK_FOREVER, &task2, &runner, false);
Task t3(2000, TASK_FOREVER, &task3, &runner, false);
Task t4(1000, TASK_FOREVER, &task4, &runner, false);
Task t5(500, TASK_FOREVER, &task5, &runner, false);

// 기본 신호등 주기 시작
void startTrafficCycle() {
    Serial.println("Starting Traffic Cycle...");
    t1.enable();
}

// 긴급 모드 ISR
void emergencyISR() {
    emergencyMode = !digitalRead(SWITCH_PIN1);
    runner.disableAll();
    Serial.println(emergencyMode ? "Emergency Mode Enabled" : "Emergency Mode Disabled");
    if (!emergencyMode) startTrafficCycle();
}

// 주의 모드 ISR
void cautionISR() {
    cautionMode = !digitalRead(SWITCH_PIN2);
    runner.disableAll();
    Serial.println(cautionMode ? "Caution Mode Enabled" : "Caution Mode Disabled");
    if (!cautionMode) startTrafficCycle();
}

// 깜빡임 모드 ISR
void blinkISR() {
    blinkMode = !digitalRead(SWITCH_PIN3);
    runner.disableAll();
    Serial.println(blinkMode ? "Blink Mode Enabled" : "Blink Mode Disabled");
    if (!blinkMode) startTrafficCycle();
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



// 초기 설정
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
    attachPCINT(digitalPinToPCINT(SWITCH_PIN3), blinkISR, CHANGE);

    Serial.println("Starting Task Scheduler...");
    
    runner.addTask(t1);
    runner.addTask(t2);
    runner.addTask(t3);
    runner.addTask(t4);
    runner.addTask(t5);

    startTrafficCycle();  // 기본 신호등 동작 시작
}

void loop() {
    portValue = analogRead(POTENTIOMETER_PIN);
    brightness = map(portValue, 0, 1023, 0, 255);

    Serial.print("Brightness: ");
    Serial.println(brightness);

    if (Serial.available() > 0) {
        String receivedData = Serial.readStringUntil('\n');
        receivedData.trim();

        if (receivedData.startsWith("TIME:")) {
            int r, y, g;
            sscanf(receivedData.c_str(), "TIME:%d,%d,%d", &r, &y, &g);
            int redTime = r;
            int yellowTime = y;
            int greenTime = g;

            Serial.print("Updated Times -> Red: ");
            Serial.print(redTime);
            Serial.print(" ms, Yellow: ");
            Serial.print(yellowTime);
            Serial.print(" ms, Green: ");
            Serial.println(greenTime);
        }
    }

    if (emergencyMode) {
        Serial.println("MODE: 긴급");
        Serial.println("TASK: 긴급 정지");
        analogWrite(RED_LED, brightness);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (cautionMode) {
        Serial.println("MODE: 주의");
        Serial.println("TASK: 모든 신호 OFF");
        analogWrite(RED_LED, 0);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (blinkMode) {
        Serial.println("MODE: 깜빡임");
        Serial.println("TASK: 모든 신호 깜빡임");
        handleBlink();
    } 
    else {
        // Serial.println("MODE: 기본");
        // Serial.println("TASK: 기본 동작 중");
        runner.execute();
    }

    delay(100); // p5.js로 너무 빠르게 전송하는 것을 방지
}


void task1() {
    Serial.println("TASK: task1 실행");  // 현재 Task 상태 전송
    analogWrite(RED_LED, brightness);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, 0);

    t1.disable();  // ✅ 현재 Task 비활성화 (다시 실행되지 않도록)
    t2.enableDelayed(2000);  // ✅ 2초 후 task2 실행
}

void task2() {
    Serial.println("TASK: task2 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, brightness);
    analogWrite(GREEN_LED, 0);

    t2.disable();  // ✅ 현재 Task 비활성화
    t3.enableDelayed(500);  // ✅ 0.5초 후 task3 실행
}

void task3() {
    Serial.println("TASK: task3 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, brightness);

    t3.disable();  // ✅ 현재 Task 비활성화
    t4.enableDelayed(2000);  // ✅ 2초 후 task4 실행
}

void task4() {
    Serial.println("TASK: task4 실행");
    for (int i = 0; i < 3; i++) {
        analogWrite(GREEN_LED, 0);
        delay(250);
        analogWrite(GREEN_LED, brightness);
        delay(250);
    }

    t4.disable();  // ✅ 현재 Task 비활성화
    t5.enableDelayed(1000);  // ✅ 1초 후 task5 실행
}

void task5() {
    Serial.println("TASK: task5 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, brightness);
    analogWrite(GREEN_LED, 0);

    t5.disable();  // ✅ 현재 Task 비활성화
    t1.enableDelayed(500);  // ✅ 0.5초 후 다시 task1 실행 (사이클 반복)
}

