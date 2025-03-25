function setup() {
  createCanvas(600, 480);
  background('lightgray');
  console.log('p5 version: v' + VERSION);
}

function draw() {
  fill(255, 0, 0);
  ellipse(mouseX, mouseY, 20);
}
