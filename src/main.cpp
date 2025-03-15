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

// 깜빡임 모드 ISR
void blinkISR() {
    blinkMode = !digitalRead(SWITCH_PIN3);
    runner.disableAll();
    Serial.println(blinkMode ? "Blink Mode Enabled" : "Blink Mode Disabled");
    if (!blinkMode) startTrafficCycle();
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

void task4() {
    Serial.println("TASK: task4 실행 (초록색 깜빡임 시작)");


    blinkMode = true;  // 🔹 깜빡임 모드 활성화
    // 🔹 깜빡임 초기화
    blinkStartTime = millis();
    blinkCount = 0;
    blinkState = false;
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

    // 🔹 BRIGHTNESS 값을 확실히 한 줄로 출력
    Serial.println("BRIGHTNESS:" + String(brightness));

    if (emergencyMode) {  // 긴급 모드
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
    else if (blinkMode) {  // 🔹 깜빡임 모드 직접 처리
        Serial.println("MODE: 깜빡임");

        if (blinkCount < 6 && currentMillis - blinkStartTime >= 250) {
            blinkStartTime = currentMillis;
            blinkState = !blinkState;
            digitalWrite(GREEN_LED, blinkState);
            blinkCount++;
        }

        if (blinkCount >= 6) {  // 3번 깜빡임 완료 후 task5 실행
            Serial.println("TASK: 깜빡임 완료, task5 실행");
            blinkMode = false;  // 🔹 깜빡임 모드 비활성화
            t4.disable();  // 🔹 task4 종료
            t5.enableDelayed(1000);
            blinkCount = 0;  // 깜빡임 횟수 초기화
        }
    } 
    else {  // 기본 신호등 주기
        runner.execute();
    }

    delay(100); // p5.js와의 통신을 위한 딜레이 설정
}