#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

// LED 핀 정의 1
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

unsigned long task1StartTime = 0; // 빨간불 시작 시간 저장
unsigned long task2StartTime = 0; // 노란불 시작 시간 저장
unsigned long task3StartTime = 0; // 초록불 시작 시간 저장
unsigned long task4StartTime = 0; // 초록불 깜빡임 시작 시간 저장
unsigned long task5StartTime = 0; // 노란불 시작 시간 저장

// 🚦 전역 변수 선언 (신호등 주기 관리)
int redDuration = 2000;   // 🔴 빨간불 기본 주기
int yellowDuration = 500;  // 🟡 노란불 기본 주기
int greenDuration = 2000;  // 🟢 초록불 기본 주기

// 전역으로 선언
bool isRedOn = false;
unsigned long redStartTime = 0;

// TaskScheduler 객체 생성
Scheduler runner;

// Task 함수 선언
void task1(); // 빨간불 켜기
void task2(); // 노란불 켜기
void task3(); // 초록불 켜기
void task4(); // 초록불 깜빡이기
void task5(); // 노란불 켜기

// Task 객체 생성 (초기에는 비활성화 상태)
Task t1(redDuration, TASK_FOREVER, &task1, &runner, false);
Task t2(yellowDuration, TASK_FOREVER, &task2, &runner, false);
Task t3(greenDuration, TASK_FOREVER, &task3, &runner, false);
Task t4(0, TASK_FOREVER, &task4, &runner, false);
Task t5(yellowDuration, TASK_FOREVER, &task5, &runner, false);

// 기본 신호등 주기 시작
void startTrafficCycle() {
    Serial.println("Starting Traffic Cycle...");
    runner.disableAll();
    isRedOn = false;  // 🔧 초기화 중요
    t1.restart();     // TaskScheduler가 바로 다음 루프에서 task1 호출
}

/* 인터럽트 서비스 루틴 (ISR) 정의 */

// 긴급 모드 ISR (스위치 1번)
void emergencyISR() {
    emergencyMode = !digitalRead(SWITCH_PIN1);
    runner.disableAll(); // 모든 태스크 비활성화
    // Serial.println(emergencyMode ? "Emergency Mode Enabled" : "Emergency Mode Disabled");

    // 🔹 p5.js로 모드 전송
    Serial.println(emergencyMode ? "MODE:Emergency" : "MODE:Normal");
    if (!emergencyMode) startTrafficCycle(); // 긴급 모드 종료 시 기본 신호등 주기 복귀
}

// 모션 감지를 통하여 모드 전환 하는 함수
void enterEmergencyMode(bool enable) {
    emergencyMode = enable;

    // 모든 task 중지
    runner.disableAll();

    Serial.println(enable ? "🚨 Emergency Mode Enabled" : "✅ Back to Normal Mode");
    Serial.print("MODE:");
    Serial.println(enable ? "Emergency" : "Normal");

    if (enable) {
        // 🔴 빨간색 LED 켜고 나머지는 끔
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
    } else {
        // 모든 LED 끄고 기본 주기로 신호등 Task 재개
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);

        startTrafficCycle();  // t1.enableDelayed(0); 같은 코드 포함
    }
}

// 주의 모드 ISR (스위치 2번)
void cautionISR() {
    cautionMode = !digitalRead(SWITCH_PIN2);
    runner.disableAll();
    // Serial.println(cautionMode ? "Caution Mode Enabled" : "Caution Mode Disabled");

    // 🔹 p5.js로 모드 전송
    Serial.println(cautionMode ? "MODE:Caution" : "MODE:Normal");

    if (cautionMode) {
        // 주의 모드에서는 모든 LED 끄기
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
    } else {
        startTrafficCycle();
    }
}

void enterCautionMode(bool enable) {
    cautionMode = enable;
    runner.disableAll();

    Serial.println(enable ? "⚠️ Caution Mode Enabled" : "✅ Back to Normal Mode");

    // 🔹 모드 상태를 p5.js로 다시 전송
    Serial.print("MODE:");
    Serial.println(enable ? "Caution" : "Normal");

    if (enable) {
        // 모든 LED 끔 (주의 모드)
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
    } else {
        // 비상 모드 해제 or 주의 모드 해제 시
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);

        // 기본 주기로 신호등 다시 실행
        startTrafficCycle();
    }
}

// 글로벌 깜빡임 모드 ISR (스위치 3번)
void blinkISR() {
    globalBlinkMode = !digitalRead(SWITCH_PIN3);
    runner.disableAll();  // 모든 Task 중지
    // Serial.println(globalBlinkMode ? "Global Blink Mode Enabled" : "Global Blink Mode Disabled");


    // 🔹 p5.js로 모드 전송
    Serial.println(globalBlinkMode ? "MODE:Global Blink" : "MODE:Normal");


    if (globalBlinkMode) {
        // 🔹 LED 초기 상태를 꺼두고 깜빡임 시작
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
    } else {
        startTrafficCycle(); // 원래 신호등 상태로 복귀
    }
}

// 모든 LED 깜빡이기 (글로벌 블링크 모드)
void handleGlobalBlink() {
    static unsigned long lastBlinkTime = 0;
    static bool state = false;
    unsigned long currentMillis = millis();

    // 🔹 500ms마다 LED 상태 전환
    if (currentMillis - lastBlinkTime >= 500) {
        lastBlinkTime = currentMillis;
        state = !state;  // 상태 반전

        // 🔹 모든 LED 깜빡이기
        digitalWrite(RED_LED, state);
        digitalWrite(YELLOW_LED, state);
        digitalWrite(GREEN_LED, state);
        Serial.print("Task:");
        Serial.println(state ? "Global Blink" : "All LEDs OFF");  // 디버깅 메시지
    }
}

// Globalblinkmode p5에서 수신 받아서 처리
void enterGlobalBlinkMode(bool enable) {
    globalBlinkMode = enable;
    runner.disableAll();

    Serial.println(enable ? "🌐 Global Blink Mode Enabled" : "✅ Back to Normal Mode");

    // 🔹 모드 정보 전송
    Serial.print("MODE:");
    Serial.println(enable ? "Global Blink" : "Normal");

    if (enable) {
        // LED 초기화 후 깜빡임 시작
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        // 깜빡임은 loop()에서 handleGlobalBlink()로 처리
    } else {
        // 모든 LED 꺼주고 기본 신호등 시작
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        startTrafficCycle();
    }
}


void handleSerialInput() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');  
        input.trim();
    
        Serial.print("📥 수신된 전체 문자열: [");
        Serial.print(input);
        Serial.println("]");

        if (input.startsWith("TIME:")) {    //p5로 부터 받은 시간값을 새로운 주기로 업데이트 시킴킴
            int newRed, newYellow, newGreen;
            sscanf(input.c_str(), "TIME:%d,%d,%d", &newRed, &newYellow, &newGreen);

            if (newRed != redDuration || newYellow != yellowDuration || newGreen != greenDuration) {    //기존값과 다를 때만 갱신신
                redDuration = newRed;
                yellowDuration = newYellow;
                greenDuration = newGreen;

                Serial.println("===== Updated Traffic Light Timings =====");
                Serial.print("🔴 Red Time: ");
                Serial.print(redDuration);  //새로운 빨간색 LED 주기 디버깅용 출력
                Serial.println(" ms");

                Serial.print("🟡 Yellow Time: ");
                Serial.print(yellowDuration);   //새로운 노란색 LED주기 디버깅용 출력
                Serial.println(" ms");

                Serial.print("🟢 Green Time: ");
                Serial.print(greenDuration);    //새로운 초록색 LED주기 디버깅용 출력
                Serial.println(" ms");
                Serial.println("======================================");

                // 모든 Task 강제 종료
                Serial.println("🔻 Stopping all tasks...");
                runner.disableAll();

                // Task 주기 업데이트 (setInterval() 사용)
                Serial.println("🔄 Updating task intervals...");
                t1.setInterval(redDuration);
                t2.setInterval(yellowDuration);
                t3.setInterval(greenDuration);
                t4.setInterval(greenDuration / 4);
                t5.setInterval(yellowDuration);

                // 3️⃣ 업데이트된 주기로 첫 Task부터 다시 실행
                Serial.println("🚦 Restarting Traffic Light Cycle...");
                runner.disableAll();  // 혹시 실행 중인 Task 모두 중단
                t1.restart();         // 강제로 처음부터 시작
                
                
                // 4️⃣ 변경된 값을 p5.js로 다시 전송하여 UI 업데이트
                Serial.print("TIME:");
                Serial.print(redDuration);
                Serial.print(",");
                Serial.print(yellowDuration);
                Serial.print(",");
                Serial.println(greenDuration);
            }
        }
        
                // 🔴 MODE: 처리
    // MODE 처리
    if (input.startsWith("MODE:")) {
        String mode = input.substring(5);
        mode.trim();  // 🔥 꼭 필요!
  
        Serial.print("💬 수신된 모드: [");
        Serial.print(mode);
        Serial.println("]");
        Serial.print("→ 길이: ");
        Serial.println(mode.length());
  
        if (mode.equals("Emergency")) {
          enterEmergencyMode(true);
        } else if (mode.equals("Caution")) {
          enterCautionMode(true);
        } else if (mode.equals("Global Blink")) {
          enterGlobalBlinkMode(true);
        } else if (mode.equals("Normal")) {
          enterEmergencyMode(false);
          enterCautionMode(false);
          enterGlobalBlinkMode(false);
        } else {
          Serial.println("⚠️ 알 수 없는 모드 수신됨!");
        }
      }
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


    if (!isRedOn) {
        redStartTime = millis();
        isRedOn = true;
        Serial.print("TASK:RED,");
        Serial.print(redDuration);
        Serial.println(" ms");

        Serial.print("TIME:");  // ✅ 현재 Task 주기 정보 전송
        Serial.print(redDuration);
        Serial.print(",");
        Serial.print(yellowDuration);
        Serial.print(",");
        Serial.println(greenDuration);

        analogWrite(RED_LED, brightness);
        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, 0);
    }

    if (millis() - redStartTime >= (unsigned long)redDuration) {
        analogWrite(RED_LED, 0);
        isRedOn = false;
        t1.disable();
        t2.enable();  // ✅ 빨간불 유지 후 노란불 Task 실행
    }
}

void task2() {    // 노란불 켜기
    static unsigned long startTime = 0;
    static bool isOn = false;

    if (!isOn) {
        task2StartTime = millis();
        Serial.print("TASK:YELLOW,");
        Serial.print(yellowDuration);
        Serial.println(" ms");

        Serial.print("TIME:");  // ✅ 현재 Task 주기 정보 전송
        Serial.print(redDuration);
        Serial.print(",");
        Serial.print(yellowDuration);
        Serial.print(",");
        Serial.println(greenDuration);

        analogWrite(YELLOW_LED, brightness);
        analogWrite(GREEN_LED, 0);

        isOn = true;
        startTime = millis();
    }

    // ✅ yellowDuration 동안 유지 후 종료
    if (millis() - startTime >= (unsigned long)yellowDuration) {
        analogWrite(YELLOW_LED, 0);  // 노란불 OFF
        isOn = false;
        t2.disable();
        t3.enable();  // ✅ 노란불 유지 후 초록불 Task 실행
    }
}


void task3() {      // 초록불 켜기
    static unsigned long startTime = 0;
    static bool isOn = false;

    if (!isOn) {
        task3StartTime = millis();
        Serial.print("TASK:GREEN,");
        Serial.print(greenDuration);
        Serial.println(" ms");

        Serial.print("TIME:");  // ✅ 현재 Task 주기 정보 전송
        Serial.print(redDuration);
        Serial.print(",");
        Serial.print(yellowDuration);
        Serial.print(",");
        Serial.println(greenDuration);

        analogWrite(YELLOW_LED, 0);
        analogWrite(GREEN_LED, brightness);

        isOn = true;
        startTime = millis();
    }

    // ✅ 초록불을 `greenDuration` 동안 유지한 후 다음 Task 실행
    if (millis() - startTime >= (unsigned long)greenDuration) {
        analogWrite(GREEN_LED, 0);  // 초록불 OFF
        isOn = false;
        t3.disable();
        t4.enable();  // ✅ 초록불 유지 후 초록불 깜빡임 Task 실행
    }
}



void task4() {      // 초록불 깜빡임
    task4StartTime = millis();
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
        // Serial.println("TASK5: Blink Done, YELLOW ON");
        blinkMode = false;
        t4.disable();
        t5.enable();
        blinkCount = 0;
    }
}

void task5() {  // 노란불 켜기
    static unsigned long startTime = 0;
    static bool isOn = false;

    if (!isOn) {
        task2StartTime = millis();
        Serial.print("TASK:YELLOW,");
        Serial.print(yellowDuration);
        Serial.println(" ms");

        Serial.print("TIME:");  // ✅ 현재 Task 주기 정보 전송
        Serial.print(redDuration);
        Serial.print(",");
        Serial.print(yellowDuration);
        Serial.print(",");
        Serial.println(greenDuration);

        analogWrite(YELLOW_LED, brightness);
        analogWrite(GREEN_LED, 0);

        isOn = true;
        startTime = millis();
    }

    // ✅ yellowDuration 동안 유지 후 종료
    if (millis() - startTime >= (unsigned long)yellowDuration) {
        analogWrite(YELLOW_LED, 0);  // 노란불 OFF
        isOn = false;
        t5.disable();
        t1.enable();  // ✅ 노란불 유지 후 빨간불 Task 실행
    }
}

unsigned long lastBrightnessSent = 0;

void loop() {
    handleSerialInput();  // 시리얼 입력 처리
    portValue = analogRead(POTENTIOMETER_PIN);
    brightness = map(portValue, 0, 1023, 0, 255);
    // 500ms마다 밝기 값 전송
    if (millis() - lastBrightnessSent > 500) {
        Serial.print("BRIGHTNESS:");
        Serial.println(brightness);
        lastBrightnessSent = millis();
    }

    if (globalBlinkMode) {
        handleGlobalBlink();  // 🔹 모든 LED 깜빡임 모드가 최우선
    } else if (emergencyMode) {
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        analogWrite(RED_LED, brightness);   // 긴급 모드 → 빨간불만 켜기
    } else if (cautionMode) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW); // 🔹 주의 모드에서는 모든 LED OFF
    } else if (blinkMode) {
        handleBlinkMode();
    } else {
        runner.execute();
    }

}