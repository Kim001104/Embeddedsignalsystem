/* 
 *************************************************************
    *  신호등 제어 시스템 (Traffic Light Control System)
    * - 3색 LED를 사용하여 신호등을 제어하는 시스템
    * - 긴급 모드, 주의 모드, 깜빡임 모드 지원
    * - 긴급 모드: 빨간 LED만 켜짐
    * - 주의 모드: 노란 LED만 켜짐
    * - 깜빡임 모드: 초록 LED만 깜빡임
    * - 글로벌 깜빡임 모드: 모든 LED 깜빡임
    * - 긴급 모드, 주의 모드, 깜빡임 모드 버튼 지원
    * - 가변 저항을 이용한 밝기 조절 기능
    * - TaskScheduler 라이브러리 사용
    * - PinChangeInterrupt 라이브러리 사용
 *************************************************************
 */

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
 
 // 상태 변수 (인터럽트 및 LED 상태 저장)
 volatile bool emergencyMode = false;  // 긴급 모드 활성화 여부
 volatile bool cautionMode = false;    // 주의 모드 활성화 여부
 volatile bool blinkMode = false;      // 초록 LED 깜빡임 모드 활성화 여부
 volatile bool globalBlinkMode = false;  // 모든 LED 깜빡임 모드 활성화 여부
 
 unsigned long blinkStartTime = 0; // 초록 LED 깜빡임 시작 시간 저장
 int blinkCount = 0; // 깜빡인 횟수 카운트
 bool blinkState = false; // 깜빡임 상태 저장
 
 // 전역 변수 선언
 int portValue = 0;
 int brightness = 0; // LED 밝기 값
 
 // TaskScheduler 객체 생성
 Scheduler runner;
 
 // Task 함수 선언
 void task1(); // 빨간불 켜기
 void task2(); // 노란불 켜기
 void task3(); // 초록불 켜기
 void task4(); // 초록불 깜빡이기
 void task5(); // 노란불 켜기
 
 // Task 객체 생성 (초기에는 비활성화 상태)
 Task t1(2000, TASK_FOREVER, &task1, &runner, false);
 Task t2(500, TASK_FOREVER, &task2, &runner, false);
 Task t3(2000, TASK_FOREVER, &task3, &runner, false);
 Task t4(0, TASK_FOREVER, &task4, &runner, false);
 Task t5(500, TASK_FOREVER, &task5, &runner, false);
 
 // 기본 신호등 주기 시작
 void startTrafficCycle() {
     Serial.println("Starting Traffic Cycle...");
     t1.enable();
 }
 
 /* 인터럽트 서비스 루틴 (ISR) 정의 */
 
 // 긴급 모드 ISR (스위치 1번)
 void emergencyISR() {
     emergencyMode = !digitalRead(SWITCH_PIN1);
     runner.disableAll(); // 모든 태스크 비활성화
     Serial.println(emergencyMode ? "Emergency Mode Enabled" : "Emergency Mode Disabled");
     if (!emergencyMode) startTrafficCycle(); // 긴급 모드 종료 시 기본 신호등 주기 복귀
 }
 
 // 주의 모드 ISR (스위치 2번)
 void cautionISR() {
     cautionMode = !digitalRead(SWITCH_PIN2);
     runner.disableAll();
     Serial.println(cautionMode ? "Caution Mode Enabled" : "Caution Mode Disabled");
     if (!cautionMode) startTrafficCycle();
 }
 
 // 글로벌 깜빡임 모드 ISR (스위치 3번)
 void blinkISR() {
     globalBlinkMode = !digitalRead(SWITCH_PIN3);
     runner.disableAll();
     Serial.println(globalBlinkMode ? "Global Blink Mode Enabled" : "Global Blink Mode Disabled");
     if (!globalBlinkMode) startTrafficCycle();
 }
 
 // 모든 LED 깜빡이기 (글로벌 블링크 모드)
 void handleGlobalBlink() {
     static unsigned long lastBlinkTime = 0;
     static bool state = false;
     unsigned long currentMillis = millis();
 
     if (currentMillis - lastBlinkTime >= 500) {
         lastBlinkTime = currentMillis;
         state = !state;
         
         digitalWrite(RED_LED, state);
         digitalWrite(YELLOW_LED, state);
         digitalWrite(GREEN_LED, state);
     }
 }
 
 // 초기 설정
 void setup() {
     Serial.begin(9600); // p5와 시리얼 통신
     
     pinMode(RED_LED, OUTPUT);   // LED 핀 출력으로 설정
     pinMode(YELLOW_LED, OUTPUT);
     pinMode(GREEN_LED, OUTPUT);
 
     pinMode(SWITCH_PIN1, INPUT_PULLUP);     // 내부 풀업 저항 사용
     pinMode(SWITCH_PIN2, INPUT_PULLUP);
     pinMode(SWITCH_PIN3, INPUT_PULLUP);
     pinMode(POTENTIOMETER_PIN, INPUT);
 
     attachInterrupt(digitalPinToInterrupt(SWITCH_PIN1), emergencyISR, CHANGE);  // 하드웨어 인터럽트
     attachInterrupt(digitalPinToInterrupt(SWITCH_PIN2), cautionISR, CHANGE);
     attachPCINT(digitalPinToPCINT(SWITCH_PIN3), blinkISR, CHANGE);  // 소프트웨어 인터럽트
 
     Serial.println("Starting Task Scheduler...");   //디버깅 확인
     runner.addTask(t1);
     runner.addTask(t2);
     runner.addTask(t3);
     runner.addTask(t4);
     runner.addTask(t5);
     startTrafficCycle();    // 초기 신호등 주기 시작
 }
 
 // Task 함수 정의 (신호등 동작 관리)
 void task1() {      // 빨간불 켜기
     Serial.println("TASK: RED ON");
     analogWrite(RED_LED, brightness);
     analogWrite(YELLOW_LED, 0);
     analogWrite(GREEN_LED, 0);
     t1.disable();
     t2.enableDelayed(2000); // 2초 후 노란불 켜기
 }
 
 void task2() {    // 노란불 켜기
     Serial.println("TASK: YELLOW ON");
     analogWrite(RED_LED, 0);
     analogWrite(YELLOW_LED, brightness);
     analogWrite(GREEN_LED, 0);
     t2.disable();
     t3.enableDelayed(500);  // 0.5초 후 초록불 켜기
 }
 
 void task3() {      // 초록불 켜기
     Serial.println("TASK: GREEN ON");
     analogWrite(RED_LED, 0);
     analogWrite(YELLOW_LED, 0);
     analogWrite(GREEN_LED, brightness);
     t3.disable();
     t4.enableDelayed(2000); // 2초 후 초록불 깜빡임
 }
 
 void task4() {      // 초록불 깜빡임
     Serial.println("TASK: GREEN BLINKING");
     blinkMode = true;
     blinkCount = 0;
     blinkStartTime = millis();
 }
 
 void handleBlinkMode() {    // 초록 LED 깜빡임 모드
     unsigned long currentMillis = millis();
     if (blinkCount < 6 && currentMillis - blinkStartTime >= 250) {
         blinkStartTime = currentMillis;
         blinkState = !blinkState;
         digitalWrite(GREEN_LED, blinkState);
         blinkCount++;
     }
     if (blinkCount >= 6) {
         Serial.println("TASK: Blink Done, YELLOW ON");
         blinkMode = false;
         t4.disable();
         t5.enableDelayed(1000); // 1초 후 노란불 켜기
         blinkCount = 0;
     }
 }
 
 void task5() {  // 노란불 켜기
     Serial.println("TASK: YELLOW ON");
     analogWrite(RED_LED, 0);
     analogWrite(YELLOW_LED, brightness);
     analogWrite(GREEN_LED, 0);
     t5.disable();
     t1.enableDelayed(500);  // 0.5초 후 빨간불 켜기
 }
 
 void loop() {
     portValue = analogRead(POTENTIOMETER_PIN);
     brightness = map(portValue, 0, 1023, 0, 255);
     Serial.println("BRIGHTNESS: " + String(brightness));
     
     if (globalBlinkMode) handleGlobalBlink();   // 모든 LED 깜빡임 모드
     else if (emergencyMode) analogWrite(RED_LED, brightness);   // 긴급 모드
     else if (cautionMode) analogWrite(YELLOW_LED, brightness);      // 주의 모드
     else if (blinkMode) handleBlinkMode();      // 초록 LED 깜빡임 모드
     else runner.execute();  // 기본 동작 실행
     
     delay(100);
 }