# RD9S를 이용한 아두이노 LED 컨트롤

## 유투브 링크
AT9과 R9DS 초기 바인딩 작업 및 AT9에서 사용자가 채널 변경과 채널 조작법 설명
https://www.youtube.com/watch?v=i5hyyABaOV4

## ✅ 아두이노 UNO 핀 할당표 (PWM + 인터럽트 최적화)

| 기능                    | 핀 번호 | 비고                                      |
|-----------------------|--------|---------------------------------------------|
| R9DS CH1               | D2     | 하드웨어 인터럽트 (attachInterrupt 사용)      |
| R9DS CH8               | D3     | 하드웨어 인터럽트 (attachInterrupt 사용)      |
| R9DS CH2               | D4     | 소프트웨어 인터럽트 (attachInterrupt 사용)    |
| 단색 LED 1 (PWM 제어)   | D5     | 하드웨어 PWM 출력 (analogWrite 사용)     |
| 단색 LED 2 (PWM 제어)   | D6     | 하드웨어 PWM 출력 (analogWrite 사용)     |
| 단색 LED 3 (PWM 제어)   | D9     | 하드웨어 PWM 출력 (analogWrite 사용)     |
| RGB LED 빨강 (PWM 제어) | D10    | 하드웨어 PWM 출력 (analogWrite 사용)     |
| RGB LED 초록 (PWM 제어) | D11    | 하드웨어 PWM 출력 (analogWrite 사용)     |
| RGB LED 파랑 (PWM 제어) | D12    | 하드웨어 PWM 출력 (analogWrite 사용)     |

## ✅ 브레드 보드 회로 구성도 

3개의 단색 LED는 **풀업저항**으로 구성하고 디지털 5,6,9핀 사용하여 PWM 지원, 
R9DS의 S(시그널)핀은 보드의 디지털 2,3,4번 핀 사용하여 하드웨어 및 소프트웨어 인터럽트 적용하고 +,-는 아두이노와 공통 단자 사용,
삼색 LED R,G,B는 디지털 9,10,11번 핀 사용하여 PWM 지원, -는 아두이노와 공통 GND 사용

![Image](https://github.com/user-attachments/assets/5516fd7e-973f-4ea3-903f-225639c67c0b)
![Image](https://github.com/user-attachments/assets/b148bce6-2edd-4e9e-af3a-2800c0e0b46e)

## ✅ R9DS 채널 기능 및 동작 정의

| 채널 번호 | 기능 설명              | 입력 장치              | 동작 규칙 및 설명 |
|-----------|----------------------|----------------------|---------------------|
| **CH1**    | LED 밝기 조절          | 조이스틱 (중앙: 1500us) | 1500us 일 때 중간 밝기, 상/하 조작으로 밝기 조절 |
| **CH8**    | LED ON/OFF 제어       | 스위치 C (중앙: 1500us) | 중앙(1500us) 이상일 때 ON, 1450us 이하일 때 OFF |
| **CH2**    | 삼색 LED 색/밝기 조절   | 조이스틱 (중앙: 1500us) | 1500us 일 때 파란색, 상/하 조작으로 밝기 실시간 조절 |

## ✅ 코드 설명

R9DS의 채널과 LED, 삼색 LED의 핀들은 배열로 설정하여서 유지 보수 용이성을 살리고
펄스 측정용 default 시간은 0으로 초기화 하고, pulse 폭의 default 값은 1500us로 설정하였다.

![Image](https://github.com/user-attachments/assets/36d48008-4136-4ec8-bb58-042c8e091502)
![Image](https://github.com/user-attachments/assets/1a4e295a-cf12-46a6-a543-cb8de6d32930)
![Image](https://github.com/user-attachments/assets/43ff1baa-6158-4fd8-aa68-f3029906e322)

위 이미지와 같이 인터럽트 핸들러 함수를 설정하고 pinchangeinterrupt의 라이브러리를 사용하여 CHANGE 모드 즉, RISING과 FALLING에서 모두 인터럽트를 처리를 하여 펄스폭을 계산하고 펄스폭에 따른 적절한 동작을 할 수 있도록 하였다.

![Image](https://github.com/user-attachments/assets/7bd3f453-8d36-4df0-8778-af4c1b1e3f76)
![Image](https://github.com/user-attachments/assets/30a9a261-1687-415b-8d54-80c00f49f947)

위 이미지와 같이 삼색 LED를 컨트롤 할 수 있는 함수를 설정하여서 삼색 LED의 밝기 변화를 나타낼 수 있도록 함수를 설정하였다. 
**색상 변경 범위는 아래 참고**

## ✅ HSV 색상환 기준 색상 범위

| Hue (도) | 색상          |
|---------|---------------|
| 0       | 빨강 (Red)     |
| 60      | 노랑 (Yellow)  |
| 120     | 초록 (Green)   |
| 180     | 청록 (Cyan)    |
| 240     | 파랑 (Blue)    |
| 300     | 자홍 (Magenta) |
| 360     | 빨강 (Red)     |



