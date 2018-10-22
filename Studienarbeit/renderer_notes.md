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
  var x = cross(x, x1-x2, y, x3-x4) / det;
  var y = cross(x, y1-y2, y, y3-y4) / det;
  return [x, y];
}

var vx1 = 0, vy1 = -25;
var vx2 = 70, vy2 = -25;
var px = 0, py = 0;
var angle =  -90 * DEGtoRAD;

var tx1 = vx1 - px, ty1 = vy1 - py;
var tx2 = vx2 - px, ty2 = vy2 - py;

var tz1 = tx1 * cos(angle) + ty1 * sin(angle);
var tz2 = tx2 * cos(angle) + ty2 * sin(angle);
var tx1 = tx1 * sin(angle) - ty1 * cos(angle);
var tx2 = tx2 * sin(angle) - ty2 * cos(angle);

var hfov = 90.0 / 2;
var nz = 0.0001, fz = 20.0;
var nx = nz / cos(hfov * DEGtoRAD);
var fx = fz / cos(hfov * DEGtoRAD);
var [ ix1, iz1 ] = intersect(tx1, tz1, tx2, tz2, -nx, nz, -fx, fz);
var [ ix2, iz2 ] = intersect(tx1, tz1, tx2, tz2, nx, nz, fx, fz);
if (tz1 <= 0.0) {
  if (iz1 > 0) {
    tx1 = ix1; tz1 = iz1;
  } else {
    tx1 = ix2; tz1 = iz2;
  }
}
if (tz2 <= 0.0) {
  if (iz1 > 0) {
    tx2 = ix1; tz2 = iz1;
  } else {
    tx2 = ix2; tz2 = iz2;
  }
}

var plane_width = 100;
var plane_height = 100;
var fov_stuff = plane_width / Math.tan(hfov * DEGtoRAD);
var height = 25;
var height_off = -0;
var x1 = -tx1 * fov_stuff / tz1,
    y1a = plane_height * (-height/2 - height_off) / tz1,
    y1b = plane_height * (height/2 - height_off) / tz1;
var x2 = -tx2 * fov_stuff / tz2,
    y2a = plane_height * (-height/2 - height_off) / tz2,
    y2b = plane_height * (height/2 - height_off) / tz2;

line(px, py, px + cos(angle) * 25, py + sin(angle) * 25, "red");
line(px, py, px -sin(angle) * 10, py + cos(angle) * 10, "#000000");
line(vx1, vy1, vx2, vy2, "#000000");

offx = 300;
line(0, 0, 0, 25, "red");
line(0, 0, -10, 0, "black");
line(tx1, tz1, tx2, tz2);

console.log(x1, x2, y1a, y2a);

offx = 700;
line (x1, y1a, x2, y2a, "black");
line (x1, y1b, x2, y2b, "black");
line (x1, y1a, x1, y1b, "red");
line (x2, y2a, x2, y2b, "red");
