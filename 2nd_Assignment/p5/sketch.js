let port;
let connectBtn, disconnectBtn;
let redSlider, yellowSlider, greenSlider;
let redLabel, yellowLabel, greenLabel;
let brightnessDisplay, modeDisplay;
let redTime = 2000;
let yellowTime = 500;
let greenTime = 2000;
let mode = "기본";
let lastTask = "None"; // 마지막 Task 값 저장 (중복 업데이트 방지)
let sendTimeout;
let timeDisplay;
let taskDisplay; // ✅ Task 상태를 표시할 HTML 요소


function setup() {
  createCanvas(windowWidth, windowHeight);
  background(220);

  port = createSerial();

  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(windowWidth/2 - 100, 10);
  connectBtn.style("background-color", "#4CAF50");
  connectBtn.size(130, 30);
  connectBtn.mousePressed(connectPort);

  disconnectBtn = createButton("Disconnect");
  disconnectBtn.position(windowWidth/2 + 100, 10);
  disconnectBtn.style("background-color", "#f44336");
  disconnectBtn.size(100, 30);
  disconnectBtn.mousePressed(disconnectPort);

  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 50);

  modeDisplay = createP("Mode: 기본");
  modeDisplay.position(10, 80);

  // ✅ Task 상태 표시 요소 다시 추가
  taskDisplay = createP("Task: None");
  taskDisplay.position(windowWidth / 2 - 110, 30);
  taskDisplay.style("font-size", "20px");
  taskDisplay.style("font-weight", "bold");
  taskDisplay.style("color", "#333");
  taskDisplay.style("padding", "10px");
  taskDisplay.style("border", "2px solid #333");
  taskDisplay.style("border-radius", "5px");
  taskDisplay.style("text-align", "center");
  taskDisplay.style("width", "300px");
  taskDisplay.style("background-color", "transparent");

  // 신호 주기 표시 요소 추가
  timeDisplay = createP("Traffic Light Timings - Red: 2000 ms, Yellow: 500 ms, Green: 2000 ms");
  timeDisplay.position(10, 110);
  timeDisplay.style("font-size", "18px");
  timeDisplay.style("font-weight", "bold");

  redSlider = createSlider(500, 5000, redTime, 10);
  redSlider.position(10, 160);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(550, 140);

  yellowSlider = createSlider(500, 5000, yellowTime, 10);
  yellowSlider.position(10, 210);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(550, 190);

  greenSlider = createSlider(500, 5000, greenTime, 10);
  greenSlider.position(10, 260);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(550, 240);
}

function draw() {
  if (port.available() > 0) {
    let str = port.readUntil("\n").trim();
    console.log("Received: ", str);

    if (str.startsWith("BRIGHTNESS:")) {
      let brightVal = parseInt(str.split(":")[1].trim());
      brightnessDisplay.html("Brightness: " + brightVal);
    } 
    else if (str.startsWith("MODE:")) {
      let modeVal = str.split(":")[1].trim();
      
      if (modeVal === "Emergency") {
        modeDisplay.html("MODE: 긴급 모드");
        modeDisplay.style("color", "red");
      } else if (modeVal === "Caution") {
        modeDisplay.html("MODE: 주의 모드");
        modeDisplay.style("color", "orange");
      } else if (modeVal === "Global Blink") {
        modeDisplay.html("MODE: 전체 깜빡임 모드");
        modeDisplay.style("color", "blue");
      } else {
        modeDisplay.html("MODE: 기본");
        modeDisplay.style("color", "#333");
      }
    }
    else if (str.startsWith("TIME:")) {    
      let times = str.substring(5).split(",");
      if (times.length === 3) {
        let newRedTime = parseInt(times[0]);
        let newYellowTime = parseInt(times[1]);
        let newGreenTime = parseInt(times[2]);

        console.log("New Times:", newRedTime, newYellowTime, newGreenTime);

        if(newRedTime !== redTime || newYellowTime !== yellowTime || newGreenTime !== greenTime) {
          redTime = newRedTime;
          yellowTime = newYellowTime;
          greenTime = newGreenTime;
    
          timeDisplay.html(
            `Traffic Light Timings - 🔴 Red: ${redTime} ms, 🟡 Yellow: ${yellowTime} ms, 🟢 Green: ${greenTime} ms`
          );
    
          redSlider.value(newredTime);
          yellowSlider.value(newyellowTime);
          greenSlider.value(newgreenTime);

          sendSignalTime();
        }
      }
    }
    else if (str.startsWith("TASK:")) {
      let taskInfo = str.substring(5);
      taskDisplay.html("Task: " + taskInfo);
    }
  }
}

// 아두이노 연결
function connectPort() {
  if (!port.opened()) {
    port.open(9600);
    console.log("Serial Port Opened");
  }
}

// 아두이노 해제
function disconnectPort() {
  if (port.opened()) {
    port.close();
    console.log("Serial Port Closed");
  }
}

// ✅ 신호 주기 업데이트
let lastRedTime = redTime;
let lastYellowTime = yellowTime;
let lastGreenTime = greenTime;

function updateRedLabel() {
  redTime = redSlider.value();
  sendSignalTime();
}

function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  sendSignalTime();
}

function updateGreenLabel() {
  greenTime = greenSlider.value();
  sendSignalTime();
}

function sendSignalTime() {
  clearTimeout(sendTimeout);
  sendTimeout = setTimeout(() => {
    let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
    console.log("Sending:", signalData);

    // 🔹 이전 값과 다를 때만 전송 (중복 방지)
    if (redTime !== lastRedTime || yellowTime !== lastYellowTime || greenTime !== lastGreenTime) {
      port.write(signalData);

      // 🔹 마지막으로 전송한 값 업데이트
      lastRedTime = redTime;
      lastYellowTime = yellowTime;
      lastGreenTime = greenTime;
    }
  }, 200);
}

// 🔹 아두이노에서 받은 주기 값을 다시 웹 UI에 반영
function updateTimingsFromArduino(red, yellow, green) {
  redTime = red;
  yellowTime = yellow;
  greenTime = green;

  timeDisplay.html(
    `Traffic Light Timings - 🔴 Red: ${redTime} ms, 🟡 Yellow: ${yellowTime} ms, 🟢 Green: ${greenTime} ms`
  );

  redSlider.value(redTime);
  yellowSlider.value(yellowTime);
  greenSlider.value(greenTime);
}
