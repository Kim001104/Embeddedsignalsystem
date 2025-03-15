#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

// LED 핀 정의
const int RED_LED = 11;     // 빨간 LED(PWM)
const int YELLOW_LED = 10;  // 노란 LED(PWM)
const int GREEN_LED = 9;    // 초록 LED(PWM)
const int SWITCH_PIN1 = 2;  // 긴급 모드 버튼
const int SWITCH_PIN2 = 3;  // 주의 모드 버튼
const int SWITCH_PIN3 = 4;  // 깜빡임 모드 버튼
const int POTENTIOMETER_PIN = A0; // 조도 조절용 가변 저항

// 상태 변수
volatile bool emergencyMode = false;  // 긴급 모드 활성화 여부
volatile bool cautionMode = false;    // 주의 모드 활성화 여부
volatile bool blinkMode = false;      // 깜빡임 모드 활성화 여부
// 🔹 새로운 변수 추가
volatile bool globalBlinkMode = false;  // 🔹 인터럽트 버튼(스위치 3)으로 모든 LED 깜빡임

unsigned long blinkStartTime = 0;
int blinkCount = 0;
bool blinkState = false;

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
Task t4(0, TASK_FOREVER, &task4, &runner, false);  // 깜빡임 주기는 millis()로 관리
Task t5(500, TASK_FOREVER, &task5, &runner, false);

// 기본 신호등 주기 시작
void startTrafficCycle() {
    Serial.println("Starting Traffic Cycle...");
    t1.enable();
}

/* 인터럽트 서브 루틴 */

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

// 🔹 인터럽트 핸들러에서 globalBlinkMode 설정
void blinkISR() {
    globalBlinkMode = !digitalRead(SWITCH_PIN3);  // 🔹 스위치를 누르면 globalBlinkMode 토글
    runner.disableAll();  // 🔹 TaskScheduler 비활성화
    Serial.println(globalBlinkMode ? "Global Blink Mode Enabled" : "Global Blink Mode Disabled");

    if (!globalBlinkMode) {
        startTrafficCycle();  // 🔹 원래 신호등 주기로 복귀
    }
}

// 🔹 스위치 3번을 눌렀을 때 모든 LED 깜빡이는 함수
void handleGlobalBlink() {
    static unsigned long lastBlinkTime = 0;
    static bool state = false;

    unsigned long currentMillis = millis();

    if (currentMillis - lastBlinkTime >= 500) {  // 500ms마다 깜빡이기
        lastBlinkTime = currentMillis;
        state = !state;
        
        digitalWrite(RED_LED, state);
        digitalWrite(YELLOW_LED, state);
        digitalWrite(GREEN_LED, state);
    }
}


// 초기 설정
void setup() {
    Serial.begin(9600); // p5와 시리얼 통신을 위한 설정

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

    startTrafficCycle();
}

// Task 함수 정의
void task1() {
    Serial.println("TASK: task1 실행");
    analogWrite(RED_LED, brightness);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, 0);
    t1.disable();
    t2.enableDelayed(2000);
}

void task2() {
    Serial.println("TASK: task2 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, brightness);
    analogWrite(GREEN_LED, 0);
    t2.disable();
    t3.enableDelayed(500);
}

void task3() {
    Serial.println("TASK: task3 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, 0);
    analogWrite(GREEN_LED, brightness);

    // 🔹 2초 동안 초록 불 유지 후 task4 실행
    t3.disable();
    t4.enableDelayed(2000);
}

// void task4() {
//     Serial.println("TASK: task4 실행 (초록색 깜빡임 시작)");


//     blinkMode = true;  // 🔹 깜빡임 모드 활성화
//     // 🔹 깜빡임 초기화
//     blinkStartTime = millis();
//     blinkCount = 0;
//     blinkState = false;
// }

void task4() {
    Serial.println("TASK: task4 실행 (초록색 깜빡임 시작)");

    blinkMode = true;  // 🔹 깜빡임 모드 활성화
    blinkCount = 0;
    blinkStartTime = millis();
}

// 🔹 초록 LED만 깜빡이는 로직
void handleBlinkMode() {
    unsigned long currentMillis = millis();

    if (blinkCount < 6 && currentMillis - blinkStartTime >= 250) {
        blinkStartTime = currentMillis;
        blinkState = !blinkState;
        digitalWrite(GREEN_LED, blinkState);
        blinkCount++;
    }

    if (blinkCount >= 6) {  // 3번 깜빡임 완료 후 task5 실행
        Serial.println("TASK: 깜빡임 완료, task5 실행");
        blinkMode = false;  // 🔹 깜빡임 모드 비활성화
        t4.disable();
        t5.enableDelayed(1000);
        blinkCount = 0;
    }
}

void task5() {
    Serial.println("TASK: task5 실행");
    analogWrite(RED_LED, 0);
    analogWrite(YELLOW_LED, brightness);
    analogWrite(GREEN_LED, 0);
    t5.disable();
    t1.enableDelayed(500);
}

void loop() {
    unsigned long currentMillis = millis();

    portValue = analogRead(POTENTIOMETER_PIN);
    brightness = map(portValue, 0, 1023, 0, 255);

    Serial.println("BRIGHTNESS:" + String(brightness));

    if (globalBlinkMode) {  // 🔹 스위치 3번으로 모든 LED 깜빡이기 (인터럽트)
        Serial.println("MODE: Global Blink");
        handleGlobalBlink();
    } 
    else if (emergencyMode) {  // 긴급 모드
        Serial.println("MODE: 긴급");
        analogWrite(RED_LED, brightness);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (cautionMode) {  // 주의 모드
        Serial.println("MODE: 주의");
        analogWrite(RED_LED, 0);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    } 
    else if (blinkMode) {  // 🔹 초록 LED만 깜빡이기
        Serial.println("MODE: Green Blink");
        handleBlinkMode();
    }
    else {  // 기본 신호등 주기
        Serial.println("MODE: Default");
        runner.execute();
    }

    delay(100); // p5.js와의 통신을 위한 딜레이 설정
}
