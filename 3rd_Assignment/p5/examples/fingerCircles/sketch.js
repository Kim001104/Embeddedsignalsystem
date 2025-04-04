const maxCircles = 40;
let   circles = [];

let video;
let handPose;
let hands = [];

function preload() {
  handPose = ml5.handPose({ flipped: true });
}

function mousePressed() {
  circles = [];
}

function gotHands(results) {
  hands = results;
}

function setup() {
  createCanvas(640, 480);

  video = createCapture(VIDEO, { flipped: true });
  video.hide();

  handPose.detectStart(video, gotHands);

  noStroke();
}

function draw() {
  image(video, 0, 0);

  // Ensure at least one hand is detected
  if (hands.length > 0) {
    let hand = hands[0];
    let index = hand.index_finger_tip;
    // console.log("🚀 ~ draw ~ index:", index)

    circles.push({ x: index.x, y: index.y, 
      size: 20+int(random(100)),
      r: int(random(255)), 
      g: int(random(255)), 
      b: int(random(255)) });

    if (circles.length > maxCircles) {
      circles.shift();
    }

    const alphaDelta = 200 / maxCircles;
    let alpha = 200 - circles.length * alphaDelta
    for (let circle of circles) {
      fill(circle.r, circle.g, circle.b, alpha);
      alpha += alphaDelta;
      ellipse(circle.x, circle.y, circle.size);
    }
  }
}
