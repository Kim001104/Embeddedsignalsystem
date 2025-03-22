let port;
let connectBtn, disconnectBtn;
let redSlider, yellowSlider, greenSlider;
let redLabel, yellowLabel, greenLabel;
let brightnessDisplay, modeDisplay;
let redTime = 2000;
let yellowTime = 500;
let greenTime = 2000;
let mode = "기본";
let sendTimeout;  // 시간 조절 슬라이더 변경 시 시리얼 통신 지연을 위한 타임아웃
let timeDisplay;  // Red, Yellow, Green 시간을 표시할 HTML 요소
let taskDisplay; // ✅ Task 상태를 표시할 HTML 요소

function setup() {
  createCanvas(windowWidth, windowHeight);
  background(220);

  port = createSerial(); // 시리얼 포트 객체 생성

  // 아두이노 연결 버튼 생성
  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(windowWidth/2 - 100, 10);
  connectBtn.style("background-color", "#4CAF50");
  connectBtn.size(130, 30);
  connectBtn.mousePressed(connectPort);

  // 아두이노 해제 버튼 생성
  disconnectBtn = createButton("Disconnect");
  disconnectBtn.position(windowWidth/2 + 100, 10);
  disconnectBtn.style("background-color", "#f44336");
  disconnectBtn.size(100, 30);
  disconnectBtn.mousePressed(disconnectPort);
  
  // 현재 실행 중인 Task 표시
  taskDisplay = createP("Task: None");
  taskDisplay.position(windowWidth/2-110, 30);
  taskDisplay.style("font-size", "20px");
  taskDisplay.style("font-weight", "bold");
  taskDisplay.style("color", "#333");
  taskDisplay.style("padding", "10px");
  taskDisplay.style("border", "2px solid #333");
  taskDisplay.style("border-radius", "5px");
  taskDisplay.style("text-align", "center");
  taskDisplay.style("width", "300px");
  taskDisplay.style("background-color", "transparent"); // 초기 배경색

  // 가변 저항 밝기 값 표시
  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 50);
  brightnessDisplay.style("font-size", "15px");
  brightnessDisplay.style("font-weight", "bold");
  brightnessDisplay.style("color", "#333");
  brightnessDisplay.size(200);

  // 현재 모드 표시
  modeDisplay = createP("MODE: 기본");
  modeDisplay.position(10, 80);
  modeDisplay.style("font-size", "20px");
  modeDisplay.style("font-weight", "bold");
  modeDisplay.style("color", "green"); // ← 기본색을 눈에 띄게
  modeDisplay.size(200);
  

  // 빨강 신호 길이 슬라이더 생성
  redSlider = createSlider(500, 5000, redTime, 10);
  redSlider.position(10, 130);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(windowWidth/2 - 300, 100);

  // 노랑 신호 길이 슬라이더 생성
  yellowSlider = createSlider(500, 5000, yellowTime, 10);
  yellowSlider.position(10, 180);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(windowWidth/2 - 300, 150);

  // 초록 신호 길이 슬라이더 생성
  greenSlider = createSlider(500, 5000, greenTime, 10);
  greenSlider.position(10, 230);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(windowWidth/2 - 300, 200);

  // 신호등 주기 정보 표시 요소 추가 (슬라이더 아래에)
  timeDisplay = createP(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
  timeDisplay.position(10, 280);
  timeDisplay.style("font-size", "16px");
  timeDisplay.style("color", "#333");
  timeDisplay.style("font-weight", "bold");

}

// let lastTask = "None"; // 마지막 Task 값 저장 (중복 업데이트 방지)

function draw() {
  if (port.available() > 0) { // 아두이노에서 시리얼 데이터를 받아서 웹 상에 html요소로 출력 
    let str = port.readUntil("\n").trim();  // 줄바꿈까지 읽어오기
    console.log("Received: ", str); // 수신한 데이터 콘솔에 출력

    if (str.startsWith("BRIGHTNESS:")) {  // BRIGHTNESS 데이터를 수신 받으면 숫자를 파싱함.
      let brightVal = parseInt(str.split(":")[1].trim()); // 밝기 값 추출
      brightnessDisplay.html("Brightness: " + brightVal); // 밝기 값 HTML 요소에 출력
    } 
    
    else if (str.includes("MODE:")) {
      let modePart = str.split("MODE:")[1]?.trim();
      console.log("📥 수신된 modeVal:", JSON.stringify(modePart));
    
      switch (modePart) {
        case "Emergency":
          modeDisplay.html("MODE: 긴급 모드");
          modeDisplay.style("color", "red");
          break;
        case "Caution":
          modeDisplay.html("MODE: 주의 모드");
          modeDisplay.style("color", "orange");
          break;
        case "Global Blink":
          modeDisplay.html("MODE: 전체 깜빡임 모드");
          modeDisplay.style("color", "blue");
          break;
        case "Normal":
          modeDisplay.html("MODE: 기본");
          modeDisplay.style("color", "green");
          break;
        default:
          console.log("⚠️ 알 수 없는 모드:", JSON.stringify(modePart));
          break;
      }
    }
            
    else if (str.startsWith("TIME:")) {     // TIME 수신 시 "TIME:" 이후 문자열만 추출해서 ["3000", "1000", "2000"]처럼 분할
      let times = str.substring(5).split(",");
      if (times.length === 3) { // 각 시간 값을 정수형 int로 분할하여 newRedTime,newYellowTime,newGreenTime에 저장장
        let newRedTime = parseInt(times[0]);
        let newYellowTime = parseInt(times[1]);
        let newGreenTime = parseInt(times[2]);

        if(newRedTime !== redTime || newYellowTime !== yellowTime || newGreenTime !== greenTime) {  //기존 값과 다를 때만 갱신신
          redTime = newRedTime;
          yellowTime = newYellowTime;
          greenTime = newGreenTime;
          timeDisplay.html(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
          redSlider.value(newRedTime);  //슬라이더 UI 변환
          yellowSlider.value(newYellowTime);
          greenSlider.value(newGreenTime);
          sendSignalTime(); // 그 후에 현재 시간값을 다시 아두이노로 보냄
        }
      }
    }
    else if (str.startsWith("TASK:")) {
      let taskInfo = str.substring(5);
      taskDisplay.html("Task: " + taskInfo);
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

function updateTimeDisplay() {
  timeDisplay.html(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
}

// 슬라이더 업데이트 함수 (빨강 신호 시간)
function updateRedLabel() {
  redTime = redSlider.value();
  redLabel.html("Red Time: " + redTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (노랑 신호 시간)
function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  yellowLabel.html("Yellow Time: " + yellowTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (초록 신호 시간)
function updateGreenLabel() {
  greenTime = greenSlider.value();
  greenLabel.html("Green Time: " + greenTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}




// 시간 조절 슬라이더 변경 시 시리얼 통신
function sendSignalTime() { 
  clearTimeout(sendTimeout);
  sendTimeout = setTimeout(() => {
    let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
    console.log("Sending:", signalData);
    port.write(signalData);
  }, 200);
}
