#include <Arduino.h>
#include <Servo.h>
#include <PinChangeInterrupt.h>

// ─────────────────────────────────────────────
// 핀 정의
const int pinSteering = 2;    // 앞뒤조절(채널2)
const int pinThrottle = 3;    // 조향조절(채널1)
const int SERVO_PIN = 9;     // 서보 제어 핀
const int ESC_PIN = 10;        // ESC 제어 핀

// ─────────────────────────────────────────────
// RC 수신값 변수
volatile int steerPulse = 1500;
volatile int throttlePulse = 1500;
volatile unsigned long steerStart = 0;
volatile unsigned long throttleStart = 0;
volatile bool newSteer = false;
volatile bool newThrottle = false;

// ─────────────────────────────────────────────
// 모터 및 서보
Servo esc;
Servo steerServo;

// ─────────────────────────────────────────────
// 인터럽트 핸들러
void isrSteer() {
  if (digitalRead(pinSteering) == HIGH) {
    steerStart = micros();
  } else {
    steerPulse = micros() - steerStart;
    newSteer = true;
  }
}

void isrThrottle() {
  if (digitalRead(pinThrottle) == HIGH) {
    throttleStart = micros();
  } else {
    throttlePulse = micros() - throttleStart;
    newThrottle = true;
  }
}

// ─────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // RC 수신기 핀 설정
  pinMode(pinSteering, INPUT_PULLUP);
  pinMode(pinThrottle, INPUT_PULLUP);
  attachPinChangeInterrupt(digitalPinToPCINT(pinSteering), isrSteer, CHANGE);
  attachPinChangeInterrupt(digitalPinToPCINT(pinThrottle), isrThrottle, CHANGE);

  // 모터/서보 초기화
  esc.attach(ESC_PIN);
  steerServo.attach(SERVO_PIN);
  esc.writeMicroseconds(1500);     // ESC 정지 상태
  steerServo.writeMicroseconds(1500); // 서보 중립 위치

  delay(3000);  // ESC 초기화 대기
  Serial.println("시작 준비 완료");
}

void loop() {
  if (newSteer || newThrottle) {
    newSteer = false;
    newThrottle = false;

    // 조향: 1/3 감도 제한
    int steerDeviation = steerPulse - 1500;           // -500 ~ +500
    int limitedSteerDeviation = steerDeviation / 3;   // -167 ~ +167
    int steerOutput = 1500 + limitedSteerDeviation;
    steerServo.writeMicroseconds(constrain(steerOutput, 1000, 2000));

    // 주행: 입력 PWM 그대로 전달
    esc.writeMicroseconds(constrain(throttlePulse, 1000, 2000));

    // 디버깅 출력
    Serial.print("Steer PWM: "); Serial.print(steerPulse);
    Serial.print(" → Output: "); Serial.print(steerOutput);
    Serial.print(" | Throttle PWM: "); Serial.print(throttlePulse);
    Serial.println();
  }
}
