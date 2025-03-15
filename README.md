# Embeddedsignalsystem


## 프로젝트 시연 영상 
[▶ YouTube 영상 보기]


## 아날로그 회로 이미지
[브레드보드 아날로그 회로 이미지]
![Image](https://github.com/user-attachments/assets/fc6dd807-61b5-401b-8be0-af45e79eadca)
[Wokwi 아날로그 회로 이미지]
![Image](https://github.com/user-attachments/assets/ef407d50-382e-436c-b1e7-436d7e044b5f)

### 회로 구성도
기본적으로 아날로그 회로를 만들 때 스위치를 풀업저항으로 만들어서 아두이노 코드에서 내부풀업을 사용할 수 있도록 만듦.
3개의 LED의 경우 아두이노 우노 보드 기준으로 디지털 9,10,11번 핀이 PWM을 사용할 수 있기에 가변 저항으로 부터 오는 신호를 PWM 변조를 하기 위해서 9,10,11번핀에 각각 위치 시켰고, 스위치의 경우 우노 보드 기준으로 2,3번 핀은 하드웨어적인 인터럽트 처리가 가능하지만 4번핀은 소프트웨어 인터럽트를 사용 해야 하기에, 스위치 1번(emergency mode)과 스위치 2번(caution mode)은 각각 디지털 2,3번 핀에 위치 시켰고 스위치3번(blink mode)의 경우 디지털 4번 핀에 위치시킴.

## 코드 

### 아두이노 코드

#### 아두이노 코드 이미지 첨부
![Image](https://github.com/user-attachments/assets/10dc4447-b8bb-4a80-a0e1-b75ba1c87452)

![Image](https://github.com/user-attachments/assets/f2ab3fb0-a4c5-4809-a390-1c24518891b5)

![Image](https://github.com/user-attachments/assets/7c573db5-d3d5-4b93-a38c-839c56d28937)

![Image](https://github.com/user-attachments/assets/4d0eb095-476a-4868-9574-f1bec81744ac)

#### 아두이노 코드 해석
본 프로젝트는 비동기적인 신호등 시스템을 만드는 것이기에 delay()함수를 써서 cpu가 불필요한 동작 혹은 인터럽트 수행 시에 원활한 인터럽트 서비스 루틴 함수로 넘어가지 못하는 것을 막기위해서 taskscheduler 라이브러르를 사용하여 비동기적인 신호등 시스템을 구현하였다.

기본 동작은 각각의 task로 나누어 자신의 task가 끝나면 그 다음에 오는 task가 와서 동작할 수 있게 하였고, task들이 진행중에 인터럽트가 생긴다면 즉각적으로 인터럽트 서비스 루틴으로 넘어가 각각의 모드를 실행 할 수 있도록 하였다.

또한 위에서 언급한 바와 같이 아두이노 우노 보드 기준으로 하드웨어적인 인터럽트를 처리할 수 있는 디지털 2,3번핀 밖에 사용을 하지 못하여 스위치 3번의 경우 소프트웨어 인터럽트를 처리하기 위해서 pinchangeinterrupt 라이브러리를 사용하여 인터럽트를 처리할 수 있게 하였다.

### p5 코드

#### p5 코드 이미지 
![Image](https://github.com/user-attachments/assets/1cbe1241-d070-4ee7-9ad1-07152cbe3b9e)

![Image](https://github.com/user-attachments/assets/276e56d2-3cd6-44aa-9272-dded56bc11b7)

![Image](https://github.com/user-attachments/assets/d85151d8-d452-425b-9115-0fe55095f71f)

![Image](https://github.com/user-attachments/assets/21fdecb1-6910-4505-812e-ab58067ea133)

#### p5 코드 설명
기본적으로 아두이노와 p5는 시리얼 통신을 지원하기 때문에 이미지와 같이 board rate(9600)으로 맞추어 주었고 기존 Arduino IDE에서 사용하던 Serial monitor와 같은 동작을 함을 볼 수 있었다. 그래서 아두이노 코드에서 Serial.print와 같은 함수를 사용하여 웹에서 동적으로 보드의 동작과 제어를 할 수 있게 설계하였다.

웹상에는 아두이노와 연결 할 수 있는 버튼을 생성하여 아두이노와 시리얼 통신을 할 수 있게 하였다.
또한, 세가지 동작을 통하여 아두이노에서 동작을 웹상에서 가시적으로 불 수 있게 하였다.
1. 가변저항 조작을 웹 상에서 볼 수 있게 함.
2. 각각의 모드가 무슨 모드인지 볼 수 있게 함.(기본동작, 비상모드, 위험 모드, 깜빡임 모드)
3. 슬라이더를 이용하여 led의 주기를 변경할 수 있게 함.
4. 현재 각각의 무슨 task를 하고 있는지 보이게 함.(task1, task2...task5)
