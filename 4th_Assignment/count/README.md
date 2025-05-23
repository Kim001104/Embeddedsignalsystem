# count

## 유투브 링크
[유투브 링크]https://www.youtube.com/shorts/FAvYFq51L_Q

## 핀맵 설명
기본적으로 라즈베리파이5의 경우 총 28가지의 GPIO핀이 있고 이 중에서 18,23,24번 핀을 사용하여 LED에 출력을 할 수 있도록 설정을 하였습니다.  
![Image](https://github.com/user-attachments/assets/ed056882-0848-4be4-bd1d-4461c4f25ebe)

## 회로 구성
![Image](https://github.com/user-attachments/assets/4c58c5a8-48d8-41d3-b6e7-3a0027d59efb)

## 코드 이미지
![Image](https://github.com/user-attachments/assets/f4b70bb0-d69b-4e58-a280-5c06173e72a8)

## 사용 방법
Raspberry Pi의 GPIO 18, 23, 24번 핀에 LED를 연결합니다.
(적절한 저항을 사용하세요.)

스크립트에 실행 권한을 부여하고 실행합니다:

chmod +x count8
./count8

## 코드 설명
**#!/usr/bin/bash는 이 스크립트를 Bash 셸에서 실행하도록 지정하는 쉐뱅(shebang)입니다.**
이 스크립트는 GPIO 18, 23, 24번 핀에 연결된 3개의 LED를 이용해,
이진수 형태로 0부터 7까지 숫자를 표시하는 LED 카운터입니다.

pins 배열을 통해 사용할 핀 번호(18, 23, 24)를 저장하고,

반복문을 통해 각각의 핀을 **출력 모드(output)**로 설정합니다.(라즈베리5에서 사용 가능한 pinctrl 함수를 통하여 출력(op)를 통하여 출력 모드 설정 가능 함.)

그 다음, i=0~7의 범위에서 1초마다 증가를 시키며 8번 반복하면서: 숫자 i를 3비트 이진수로 변환해 각 비트를 추출하고,

추출된 비트가 1이면 라즈베리파이5에서 동작하는 **pinctrl 함수**를 통해서 해당 핀(GPIO)을 HIGH(ON), 0이면 LOW(OFF) 하여 LED 상태를 결정합니다.

최종적으로 모든 LED는 OFF 상태로 종료됩니다.

<추가 설명>
| i | 이진수 | LED2 (pin 18) | LED1 (pin 23) | LED0 (pin 24) |
|---|--------|----------------|----------------|----------------|
| 0 | 000    | ⚫ OFF          | ⚫ OFF          | ⚫ OFF          |
| 1 | 001    | ⚫ OFF          | ⚫ OFF          | 🔴 ON           |
| 2 | 010    | ⚫ OFF          | 🔴 ON           | ⚫ OFF          |
| 3 | 011    | ⚫ OFF          | 🔴 ON           | 🔴 ON           |
| 4 | 100    | 🔴 ON           | ⚫ OFF          | ⚫ OFF          |
| 5 | 101    | 🔴 ON           | ⚫ OFF          | 🔴 ON           |
| 6 | 110    | 🔴 ON           | 🔴 ON           | ⚫ OFF          |
| 7 | 111    | 🔴 ON           | 🔴 ON           | 🔴 ON           |
