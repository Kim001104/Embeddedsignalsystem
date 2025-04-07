# domino

## 유투브 링크
[유투브 링크]https://www.youtube.com/shorts/YXX1ydl80u4

## 핀맵 설명
기본적으로 라즈베리파이5의 경우 총 28가지의 GPIO핀이 있고 이 중에서 18,23,24,25번 핀을 사용하여 LED에 출력을 할 수 있도록 설정을 하였습니다. 
![Image](https://github.com/user-attachments/assets/91a40300-df88-41c1-b4ad-d3401d6f2902)

## 회로 구성
![Image](https://github.com/user-attachments/assets/037cb5ed-d59f-4b2b-959f-78ac4cbb5bd3)

## 코드 이미지 
![Image](https://github.com/user-attachments/assets/04935a72-5269-411e-b8c6-6569e8727123)

## 사용 방법
Raspberry Pi의 GPIO 18, 23, 24번 핀에 LED를 연결합니다.
(적절한 저항을 사용하세요.)

스크립트에 실행 권한을 부여하고 실행합니다:

chmod +x domino4
./domino4

## 코드 설명 
**#!/usr/bin/bash는 이 스크립트를 Bash 셸에서 실행하도록 지정하는 쉐뱅(shebang)입니다.**
이 스크립트는 GPIO 18, 23, 24, 25번 핀에 연결된 4개의 LED를 순차적으로 깜빡이게 만드는 도미노 LED 제어 스크립트입니다.

pins 배열에 사용할 GPIO 핀 번호를 저장하고,

첫 번째 for 반복문에서 각 핀을 **출력 모드(output)**로 설정합니다.(라즈베리 파이5의 경우 pinctrl 함수를 통해서 출력(op)를 설정 가능능)

이후 while 루프 안에서: 각 핀에 대해 1초간 켜졌다가 꺼지는 동작을 반복하며,

LED들이 도미노처럼 차례대로 깜빡이는 효과를 줍니다.(이 또한 pinctrl 함수를 통해서 dh(HIGH)와 dl(LOW)를 설정 가능 함.)

