let port; // 시리얼 포트 객체
let connectBtn; // 아두이노 연결 버튼
let redSlider, yellowSlider, greenSlider; // 신호등 지속 시간을 조절하는 슬라이더
let redLabel, yellowLabel, greenLabel; // 각 슬라이더의 값을 표시하는 텍스트
let brightnessDisplay; // 가변 저항 밝기 표시
let modeDisplay; // 현재 모드 표시
let redTime = 2000; // 빨간불 지속 시간 (기본값: 2000ms)
let yellowTime = 500; // 노란불 지속 시간 (기본값: 500ms)
let greenTime = 2000; // 초록불 지속 시간 (기본값: 2000ms)
let mode = "기본"; // 현재 모드 (기본값: 기본 모드)

function setup() {
  createCanvas(windowWidth, windowHeight); // 창 크기에 맞게 캔버스 생성
  background(220);

  // 시리얼 포트 설정
  port = createSerial();
  let usedPorts = usedSerialPorts();
  if (usedPorts.length > 0) {
    port.open(usedPorts[0], 9600); // 사용 가능한 포트가 있으면 자동 연결
  }

  // 아두이노 연결 버튼 생성 및 설정
  connectBtn = createButton("아두이노 연결");
  connectBtn.position(500, 10);
  connectBtn.mousePressed(connectBtnClick);

  // 빨간불 지속 시간 슬라이더 및 라벨 생성
  redSlider = createSlider(500, 5000, redTime, 100);
  redSlider.position(10, 50);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(10, 70);
  
  // 노란불 지속 시간 슬라이더 및 라벨 생성
  yellowSlider = createSlider(500, 5000, yellowTime, 100);
  yellowSlider.position(10, 110);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(10, 130);
  
  // 초록불 지속 시간 슬라이더 및 라벨 생성
  greenSlider = createSlider(500, 5000, greenTime, 100);
  greenSlider.position(10, 170);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(10, 190);

  // 가변 저항 밝기 값 표시
  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 240);

  // 현재 모드 표시
  modeDisplay = createP("Mode: 기본");
  modeDisplay.position(10, 270);
  
  // 모드 변경 버튼 생성
  createButton("기본 동작").position(10, 310).mousePressed(() => changeMode("기본"));
  createButton("비상").position(90, 310).mousePressed(() => changeMode("비상"));
  createButton("위험").position(150, 310).mousePressed(() => changeMode("위험"));
  createButton("깜빡임").position(210, 310).mousePressed(() => changeMode("깜빡임"));
}

function draw() {
  let n = port.available(); // 시리얼 데이터가 들어왔는지 확인
  if (n > 0) {
    let str = port.readUntil("\n").trim(); // 한 줄 단위로 데이터 읽기
    background(220);
    fill(0);
    text("msg: " + str, 10, 350); // 디버깅용 메시지 출력

    // 밝기 값 수신 및 표시
    if (str.startsWith("Brightness:")) {
      let brightVal = str.split(":")[1].trim();
      brightnessDisplay.html("Brightness: " + brightVal);
    } 
    // 모드 변경 정보 수신 및 표시
    else if (str.startsWith("MODE:")) {
      mode = str.split(":")[1].trim();
      modeDisplay.html("Mode: " + mode);
    }
  }
}

// 아두이노 연결/해제 함수
function connectBtnClick() {
  if (!port.opened()) {
    port.open(9600);
  } else {
    port.close();
  }
}

// 빨간불 지속 시간 업데이트
function updateRedLabel() {
  redTime = redSlider.value();
  redLabel.html("Red Time: " + redTime + " ms");
  changeSignalTime(); // 변경된 값을 아두이노로 전송
}

// 노란불 지속 시간 업데이트
function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  yellowLabel.html("Yellow Time: " + yellowTime + " ms");
  changeSignalTime(); // 변경된 값을 아두이노로 전송
}

// 초록불 지속 시간 업데이트
function updateGreenLabel() {
  greenTime = greenSlider.value();
  greenLabel.html("Green Time: " + greenTime + " ms");
  changeSignalTime(); // 변경된 값을 아두이노로 전송
}

// 변경된 신호등 시간을 아두이노로 전송
function changeSignalTime() {
  let signalData = `${redTime},${yellowTime},${greenTime}\n`;
  port.write(signalData);
}

// 모드 변경 요청을 아두이노로 전송
function changeMode(newMode) {
  port.write("MODE:" + newMode + "\n");
}
