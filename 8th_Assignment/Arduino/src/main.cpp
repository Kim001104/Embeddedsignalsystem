#include <Arduino.h>
#include <PinChangeInterrupt.h>

// 채널 핀 정의
const byte chPins[3] = {2, 3, 4};
const byte ledPins[3] = {9, 10, 11};
const byte rgbPins[3] = {5, 6, 7};

// 펄스 측정용 변수
volatile unsigned long startTimes[3] = {0, 0, 0};
volatile unsigned long pulseWidths[3] = {1500, 1500, 1500};  // 초기값 1500us


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
    startTimes[idx] = micros();
  } else {
    unsigned long width = micros() - startTimes[idx];
    if (width >= 800 && width <= 2200) {  // 유효 범위 체크
      pulseWidths[idx] = width;
    }
  }
}

// ISR 핸들러 배열 정의
void handleCh1() { measurePulse(0); }
void handleCh2() { measurePulse(1); }
void handleCh3() { measurePulse(2); }

void (*isrHandlers[3])() = {handleCh1, handleCh2, handleCh3};


void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 3; i++) {
    pinMode(chPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    pinMode(rgbPins[i], OUTPUT);
    attachPCINT(digitalPinToPCINT(chPins[i]), isrHandlers[i], CHANGE);
  }
}

void loop() {
  unsigned long pulse1 = pulseWidths[0];
  unsigned long pulse2 = pulseWidths[1];
  unsigned long pulse3 = pulseWidths[2];

  int brightness = constrain(map(pulse1, 1000, 2000, 0, 255), 0, 255);
  bool ledOn = pulse2 > 1400;

  float hue = map(pulse3, 1050, 1850, 0, 180);  // 0-180 범위로 변환
  float r, g, b;
  hsvToRgb(hue, 1.0, 1.0, &r, &g, &b);

  for (int i = 0; i < 3; i++) {
    analogWrite(ledPins[i], ledOn ? brightness : 0);
  }

  analogWrite(rgbPins[0], ledOn ? int(r * 255) : 0);
  analogWrite(rgbPins[1], ledOn ? int(g * 255) : 0);
  analogWrite(rgbPins[2], ledOn ? int(b * 255) : 0);

  Serial.print("CH1: "); Serial.print(pulse1);
  Serial.print(" | CH2: "); Serial.print(pulse2);
  Serial.print(" | CH3: "); Serial.println(pulse3);

  delay(100);
}


