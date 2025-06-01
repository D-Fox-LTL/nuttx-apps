let autoMode = false;

function cmd(name) {
  fetch('/' + name)
    .then(response => console.log(name + ' sent'));
}

function toggleAuto() {
  autoMode = !autoMode;
  fetch('/auto?set=' + (autoMode ? 'on' : 'off'));
}mode == MODE_AUTO

function scan() {
  fetch('/scan')
    .then(response => response.json())
    .then(data => drawRadar(data));
}

function drawRadar(distances) {
  // Simple radial lines from center
  let canvas = document.getElementById('radarCanvas');
  let ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  let cx = canvas.width / 2;
  let cy = canvas.height;
  let maxRadius = cy - 10;
  let steps = distances.length;
  for (let i = 0; i < steps; i++) {
    let angle = Math.PI * (i / (steps - 1)); // 0 to π radians
    let dist = Math.min(distances[i], 100); // clamp max
    let r = (dist / 100) * maxRadius;
    let x = cx + r * Math.sin(angle);
    let y = cy - r * Math.cos(angle);
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(x, y);
    ctx.strokeStyle = 'red';
    ctx.stroke();
  }
}
