#include <Arduino.h>
#include <PinChangeInterrupt.h>

// 채널 핀 정의
const byte chPins[3] = {2, 3, 4}; // 채널 1, 2, 3
const byte ledPins[3] = {9, 10, 11};  // LED 핀
const byte rgbPins[3] = {5, 6, 7};  // 삼색 LED 핀

// 펄스 측정용 변수
volatile unsigned long startTimes[3] = {0, 0, 0}; // 시작 시간 초기화
volatile unsigned long pulseWidths[3] = {1500, 1500, 1500};  // 기본값 1500us 펄스 폭


// HSV to RGB 변환 함수
void hsvToRgb(float h, float s, float v, float* r, float* g, float* b) {
  int i = int(h / 60.0) % 6;
  float f = h / 60.0 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i) {
    case 0: *r = v; *g = t; *b = p; break;
    case 1: *r = q; *g = v; *b = p; break;
    case 2: *r = p; *g = v; *b = t; break;
    case 3: *r = p; *g = q; *b = v; break;
    case 4: *r = t; *g = p; *b = v; break;
    case 5: *r = v; *g = p; *b = q; break;
  }
}

// 펄스 측정 함수
void measurePulse(byte idx) {
  if (digitalRead(chPins[idx]) == HIGH) {
    startTimes[idx] = micros(); // HIGH 상태에서 시작 시간 기록
  } else {
    unsigned long width = micros() - startTimes[idx]; // LOW 상태에서 펄스 폭 계산
    if (width >= 800 && width <= 2200) {  // 안전성을 위해서 800,2200us 범위로 제한
      pulseWidths[idx] = width; // 펄스 폭 저장
    }
  }
}

// ISR 핸들러 배열 정의
void handleCh1() { measurePulse(0); } //채널1의 핸들러
void handleCh2() { measurePulse(1); } //채널2의 핸들러
void handleCh3() { measurePulse(2); } //채널3의 핸들러

void (*isrHandlers[3])() = {handleCh1, handleCh2, handleCh3}; // ISR 핸들러 배열


void setup() {
  Serial.begin(9600); // 시리얼 통신 시작
  
  for (int i = 0; i < 3; i++) { // 각 채널에 대해 핀 모드 설정
    pinMode(chPins[i], INPUT);  // 채널 핀 입력 모드
    pinMode(ledPins[i], OUTPUT);  // LED 핀 출력 모드
    pinMode(rgbPins[i], OUTPUT);  // 삼색 LED 핀 출력모드 
    attachPCINT(digitalPinToPCINT(chPins[i]), isrHandlers[i], CHANGE);  // 인터럽트 설정 CHANGE를 해야지 HIGH와 LOW 모두 감지하여 펄스 폭 계산 
  }
}

void loop() {
  unsigned long pulse1 = pulseWidths[0];  // 펄스 폭을 읽어옴
  unsigned long pulse2 = pulseWidths[1];  
  unsigned long pulse3 = pulseWidths[2]; 

  int brightness = constrain(map(pulse1, 1000, 2000, 0, 255), 0, 255);  // 0-255 범위로 변환
  bool ledOn = pulse2 > 1400; // 채널 2의 펄스 폭에 따라 LED ON/OFF 결정(1400us 이상이면 ON)

  float hue = map(pulse3, 1050, 1850, 0, 180);  // 0-180 범위로 변환
  float r, g, b;
  hsvToRgb(hue, 1.0, 1.0, &r, &g, &b);  // HSV를 RGB로 변환

  for (int i = 0; i < 3; i++) { 
    analogWrite(ledPins[i], ledOn ? brightness : 0);  // LED 밝기 조절(삼색 LED와는 별개)
  }

  analogWrite(rgbPins[0], ledOn ? int(r * 255) : 0);  // 삼색 LED 색상 조절
  analogWrite(rgbPins[1], ledOn ? int(g * 255) : 0);
  analogWrite(rgbPins[2], ledOn ? int(b * 255) : 0);

  Serial.print("CH1: "); Serial.print(pulse1);
  Serial.print(" | CH2: "); Serial.print(pulse2);
  Serial.print(" | CH3: "); Serial.println(pulse3);

  delay(100);
}


