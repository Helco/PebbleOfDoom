var canvas = document.getElementById('myCanvas');
var context = canvas.getContext('2d');

var offx = 100, offy = 100;
function line(x1, y1, x2, y2, color)
{
  context.strokeStyle=color;
  context.lineWidth = 3;
  context.beginPath()
  context.moveTo(offx + x1, offy + y1);
  context.lineTo(offx + x2, offy + y2);
  context.stroke();
}
var cos = Math.cos;
var sin = Math.sin;
var DEGtoRAD = Math.PI / 180.0;

function cross(x1,y1, x2,y2) {
  return x1*y2 - y1*x2;
}

function intersect(x1,y1, x2,y2, x3,y3, x4,y4)
{
  var x = cross(x1, y1, x2, y2);
  var y = cross(x3, y3, x4, y4);
  var det = cross(x1-x2, y1-y2, x3-x4, y3-y4);
  var ix = cross(x, x1-x2, y, x3-x4) / det;
  var iy = cross(x, y1-y2, y, y3-y4) / det;
  return [ix, iy];
}

var vx1 = 0, vy1 = 25*4;
var vx2 = 70, vy2 = 25*4;
var px = 30, py = -20;
var angle = 130 * DEGtoRAD;
setInterval(() => { angle += 1 * DEGtoRAD; drawstuff(); }, 16);

function drawstuff() {
  context.clearRect(0, 0, canvas.width, canvas.height);
  
var tx1 = vx1 - px, ty1 = vy1 - py;
var tx2 = vx2 - px, ty2 = vy2 - py;

var tz1 = tx1 * cos(angle) + ty1 * sin(angle);
var tz2 = tx2 * cos(angle) + ty2 * sin(angle);
var tx1 = tx1 * sin(angle) - ty1 * cos(angle);
var tx2 = tx2 * sin(angle) - ty2 * cos(angle);
var u1 = 0.0, u2 = 1.0;

function xchange(a, b) {
  var tmp = a;
  a = b;
  b = tmp;
  return [a, b];
}
if (tx2 > tx1) {
  [tx1, tx2] = xchange(tx1, tx2);
  [tz1, tz2] = xchange(tz1, tz2);
}


var hfov = 90.0 / 2;
var nz = 0.0001, fz = 100.0;
var nx = nz / cos(hfov * DEGtoRAD);
var fx = fz / cos(hfov * DEGtoRAD);
var [ ix1, iz1 ] = intersect(tx1, tz1, tx2, tz2, -nx, nz, -fx, fz);
var [ ix2, iz2 ] = intersect(tx1, tz1, tx2, tz2, nx, nz, fx, fz);

var otx1 = tx1, otx2 = tx2;
var rw1 = (ix2 - tx1) / (tx2 - tx1);
if (rw1 >= 0.0 && rw1 <= 1.0) {
  tx1 = ix2;
  tz1 = iz2;
  u1 = rw1;
}
var rw2 = (ix1 - otx1) / (otx2 - otx1); // don't overwrite tx1 before
if (rw2 >= 0.0 && rw2 <= 1.0) {
  tx2 = ix1;
  tz2 = iz1;
  u2 = rw2;
}
if ((iz1 < 0 && iz2 > 0 && tz1 < iz2 && tz2 < iz2) ||
    (iz2 < 0 && iz1 > 0 && tz1 < iz1 && tz2 < iz1))
  {
    tz1 = tz2 = -1 / 0;
  }

var plane_width = 200;
var plane_height = 100;
var fov_stuff = (plane_width * Math.cos(hfov * DEGtoRAD)) / Math.tan(hfov * DEGtoRAD);
var height = 25;
var height_off = -0;
var x1 = -tx1 * fov_stuff / tz1,
    y1a = plane_height * (-height/2 - height_off) / tz1,
    y1b = plane_height * (height/2 - height_off) / tz1;
var x2 = -tx2 * fov_stuff / tz2,
    y2a = plane_height * (-height/2 - height_off) / tz2,
    y2b = plane_height * (height/2 - height_off) / tz2;
  
offx = 0;
line(px, py, px + cos(angle) * 25, py + sin(angle) * 25, "red");
line(px, py, px -sin(angle) * 10, py + cos(angle) * 10, "#000000");
line(vx1, vy1, vx2, vy2, "#000000");
  
offx = 300;
line(0, 0, 0, 25, "red");
line(0, 0, -10, 0, "black");
line(tx1, tz1, tx2, tz2, "black");
line(nx, nz, fx, fz, "blue");
line(-nx, nz, -fx, fz, "blue");
line (ix1, iz1 - 3, ix1, iz1 + 3, "green");
line (ix2, iz2 - 3, ix2, iz2 + 3, "green");

offx = 800;
line (-plane_width, 0, +plane_width, 0, "black");
if (tz1 > 0 && tz2 > 0) {
line (x1, y1a, x2, y2a, "black");
line (x1, y1b, x2, y2b, "black");
line (x1, y1a, x1, y1b, "red");
line (x2, y2a, x2, y2b, "red");
  
for (var i = 0.1; i <= 0.9; i+=0.1)
  {
    var u = u1 + (u2 - u1) * i;
    var lx = x1 + (x2 - x1) * i;
    var ly1 = y1a + (y2a - y1a) * i;
    var ly2 = y1b + (y2b - y1b) * i;
    line(lx, ly1, lx, ly2, `hsl(${ Math.floor(u * 300 + 50) }, 100%, 50%)`);
  }
}
}