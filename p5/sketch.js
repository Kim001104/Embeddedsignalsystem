let port;
let connectBtn, disconnectBtn;
let redSlider, yellowSlider, greenSlider;
let redLabel, yellowLabel, greenLabel;
let brightnessDisplay, modeDisplay;
let redTime = 2000;
let yellowTime = 500;
let greenTime = 2000;
let mode = "기본";

function setup() {
  createCanvas(windowWidth, windowHeight);
  background(220);

  port = createSerial();

  // 아두이노 연결 버튼
  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(windowWidth/2 - 100, 10);
  connectBtn.style("background-color", "#4CAF50");
  connectBtn.size(130, 30);
  connectBtn.mousePressed(connectPort);

  // 아두이노 해제 버튼
  disconnectBtn = createButton("Disconnect");
  disconnectBtn.position(windowWidth/2 + 100, 10);
  disconnectBtn.style("background-color", "#f44336");
  disconnectBtn.size(100, 30);
  disconnectBtn.mousePressed(disconnectPort);
  
  // 현재 실행 중인 Task 표시
  taskDisplay = createP("Task: None");
  taskDisplay.position(windowWidth/2-85, 30);
  taskDisplay.style("font-size", "20px");
  taskDisplay.style("font-weight", "bold");
  taskDisplay.style("color", "#333");
  taskDisplay.style("padding", "10px");
  taskDisplay.style("border", "2px solid #333");
  taskDisplay.style("border-radius", "5px");
  taskDisplay.style("text-align", "center");
  taskDisplay.style("width", "250px");
  taskDisplay.style("background-color", "transparent"); // 초기 배경색


  // 밝기 값 표시
  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 50);
  brightnessDisplay.style("font-size", "20px");
  brightnessDisplay.style("font-weight", "bold");
  brightnessDisplay.style("color", "#333");
  brightnessDisplay.size(200);

  // 현재 모드 표시
  modeDisplay = createP("Mode: 기본");
  modeDisplay.position(10, 80);
  modeDisplay.style("font-size", "20px");
  modeDisplay.style("font-weight", "bold");
  modeDisplay.style("color", "#333");
  modeDisplay.size(200);

  

  // 빨강 신호 길이 슬라이더
  redSlider = createSlider(500, 5000, redTime, 10);
  redSlider.position(10, 130);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(windowWidth/2 - 300, 100);

  // 노랑 신호 길이 슬라이더
  yellowSlider = createSlider(500, 5000, yellowTime, 10);
  yellowSlider.position(10, 180);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(windowWidth/2 - 300, 150);

  // 초록 신호 길이 슬라이더
  greenSlider = createSlider(500, 5000, greenTime, 10);
  greenSlider.position(10, 230);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(windowWidth/2 - 300, 200);
}

let lastTask = "None"; // 마지막 Task 값 저장 (중복 업데이트 방지)

function draw() {
  if (port.available() > 0) {
    let str = port.readUntil("\n").trim(); // 시리얼 데이터 읽기

    if (str.startsWith("Brightness:")) {
      let brightVal = parseInt(str.split(":")[1].trim());
      brightnessDisplay.html("Brightness: " + brightVal);
    } 
    else if (str.startsWith("MODE:")) {
      let modeVal = str.split(":")[1].trim();
      modeDisplay.html("Mode: " + modeVal);
    } 
    else if (str.startsWith("TASK:")) {
      let taskVal = str.split(":")[1].trim();

      // 기존 Task와 다를 때만 업데이트 (불필요한 갱신 방지)
      if (taskVal !== lastTask) {
        lastTask = taskVal; // 최신 Task 값 저장
        taskDisplay.html("Task: " + taskVal);

        // Task 변경 시 강조 효과 추가
        taskDisplay.style("background-color", "red"); // 강조
        // setTimeout(() => taskDisplay.style("background-color", "transparent"), 500); // 0.5초 후 원래 색으로 복귀
      }
    }
  }
}

// 아두이노 연결 함수
function connectPort() {
  if (!port.opened()) {
    port.open(9600);
    console.log("Serial Port Opened");
  }
}

// 아두이노 해제 함수
function disconnectPort() {
  if (port.opened()) {
    port.close();
    console.log("Serial Port Closed");
  }
}

// 슬라이더 업데이트 함수 (빨강 신호 시간)
function updateRedLabel() {
  redTime = redSlider.value();
  redLabel.html("Red Time: " + redTime + " ms");
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (노랑 신호 시간)
function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  yellowLabel.html("Yellow Time: " + yellowTime + " ms");
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (초록 신호 시간)
function updateGreenLabel() {
  greenTime = greenSlider.value();
  greenLabel.html("Green Time: " + greenTime + " ms");
  sendSignalTime();
}

// 아두이노로 신호 시간 전송
function sendSignalTime() {
  let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
  port.write(signalData);
}
