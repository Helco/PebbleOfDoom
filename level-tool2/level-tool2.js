const fs = require("fs");
const path = require("path");
const yaml = require("yaml");
const Format = require("bin-format");

const FormatVector = new Format()
    .floatLE("x")
    .floatLE("y");

const FormatLocation = new Format()
    .int32LE("sector")
    .nest("position", FormatVector)
    .floatLE("height")
    .floatLE("angle");

const FormatSector = new Format()
    .int32LE("wallOffset")
    .int32LE("wallCount")
    .int32LE("height")
    .int32LE("heightOffset")
    .uint8("floorColor")
    .uint8("ceilColor");

const FormatWall = new Format()
    .int32LE("startCorner")
    .int32LE("portalTo")
    .int32LE("texture")
    .nest("texStart", FormatVector)
    .nest("texEnd", FormatVector)
    .uint8("color")
    .uint8("flags")

const FormatLevel = new Format()
    .int32LE("storageVersion")
    .int32LE("sectorCount")
    .int32LE("wallCount")
    .int32LE("vertexCount")
    .nest("playerStart", FormatLocation)
    .listEof("vertices", FormatVector)
    .listEof("sectors", FormatSector)
    .listEof("walls", FormatWall);

const level = {
    storageVersion: 4,
    sectorCount: 1,
    wallCount: 1,
    vertexCount: 2,
    playerStart: {
        sector: 0,
        position: {
            x: 3.4,
            y: 1.2
        },
        height: 30,
        angle: 20
    },
    vertices: [
        {
            x: 1,
            y: 2
        },
        {
            x: 3,
            y: 4
        }
    ],
    sectors: [
        {
            wallOffset: 0,
            wallCount: 1,
            height: 40,
            heightOffset: 30,
            floorColor: 0xef,
            ceilColor: 0xfe
        }
    ],
    walls: [
        {
            startCorner: 0,
            portalTo: -1,
            texture: -18,
            texStart: {
                x: 0,
                y: 0
            },
            texEnd: {
                x: 1,
                y: 1
            },
            color: 0xff,
            flags: 0,
        }
    ]
};

const descr = yaml.parse(fs.readFileSync("level-tool2/level.yaml", "utf-8"));
level.vertices = [];
level.sectors = [];
level.walls = [];

let evalEnv = "(()=>{";
for (var v in descr.vars)
    evalEnv += `const ${v} = (${descr.vars[v]});`;
evalEnv += "return (";
function reportError(f, name) {
    try
    {
        return f();
    }
    catch(e)
    {
        throw "Have error parsing " + name + ": " + e;
    }
}
function value(str, name) { return reportError(() => eval(evalEnv + str + ");})()"), name); }
function ivalue(str, name) { return reportError(() => parseInt(value(str, name)), name); }
function fvalue(str, name) { return reportError(() => parseFloat(value(str, name)), name); }
function vecvalue(str, name) { return reportError(() => {
    const parts = str.split(",").map(p => p.trim()).filter(p => p !== "");
    if (parts.length != 2)
        throw "invalid part count for vector";
    return {
        x: fvalue(parts[0], name + "_x"),
        y: fvalue(parts[1], name + "_y")
    };
}, name); }
function colvalue(str, name) { return reportError(() => {
    const parts = str.split(",").map(p => p.trim()).filter(p => p !== "");
    if (parts.length != 3)
        throw "invalid part count for color " + parts.length;
    const col = {
        r: fvalue(parts[0], name + "_r"),
        g: fvalue(parts[1], name + "_g"),
        b: ivalue(parts[2], name + "_b")
    };
    function isComp(v) { return col.r >= 0 && col.r <= 3 && (col.r|0) === col.r; }
    if (!isComp(col.r)) throw "invalid value in r " + col.r;
    if (!isComp(col.g)) throw "invalid value in g " + col.g;
    if (!isComp(col.b)) throw "invalid value in b " + col.b;
    return (col.r * (1 << 0)) +
        (col.g * (1 << 2)) +
        (col.b * (1 << 4)) +
        (3 * (1 << 6));
}, name); }
function splitvalue(str, name, parts, sep) { return reportError(() => {
    const parts = str.split(sep).map(p => p.trim()).filter(p => p !== "");
    if (parts.length !== parts)
        throw "invalid part count";
    return parts;
}, name); }

for (var v in descr.vertices)
{
    descr.vertices[v] = vecvalue(descr.vertices[v], "vertex " + v);
    descr.vertices[v].index = level.vertices.length;
    level.vertices.push(descr.vertices[v]);
}

const defaultFloorColor = colvalue(descr.defaultFloorColor, "defaultFloorColor");
const defaultCeilColor = colvalue(descr.defaultCeilColor, "defaultCeilColor");

const portalMap = new Map();
const cornerMap = new Map();

// First pass, normalize walls
for (var sectorName in descr.sectors)
{
    const sector = descr.sectors[sectorName];
    sector.height = sector.h = fvalue(sector.h, sectorName + "_h");
    sector.heightOffset = sector.y = fvalue(sector.y, sectorName + "_h");
    sector.yEnd = sector.y + sector.h;
    sector.floorColor = ("floorColor" in sector) && sector["floorColor"] !== undefined
        ? colvalue(sector.floorColor, sectorName + "_floorColor")
        : defaultFloorColor;
    sector.ceilColor = ("ceilColor" in sector) && sector["ceilColor"] !== undefined
        ? colvalue(sector.ceilColor, sectorName + "_ceilColor")
        : defaultCeilColor;

    sector.wallOffset = level.walls.length;
    sector.wallCount = sector.walls.length;
    sector.index = level.sectors.length;
    level.sectors.push(sector);

    //sector.walls = sector.walls.reverse();

    for (var i = 0; i < sector.walls.length; i++)
    {
        let w = sector.walls[i];
        if (typeof(w) === "string")
            w = { corner: "" + w };
        
        let vertex = {};
        if ((w.corner.trim()) in descr.vertices)
            vertex = descr.vertices[w.corner.trim()];
        else
        {
            vertex = vecvalue(w.corner, `${sectorName}_wall${i}_corner`);
            vertex.index = level.vertices.length;
            level.vertices.push(vertex);
        }

        w.corner = vertex;
        w.startCorner = vertex.index;
        w.portalTo = -1;
        w.texture = -1;
        w.color = 0xff;
        w.texStart = { x: 0, y: 0 };
        w.texEnd = { x: 1, y: 1 };
        w.index = sector.wallOffset + i;
        sector.walls[i] = w;
        level.walls.push(w);
    }

    for (var i = 0; i < sector.walls.length; i++)
    {
        var v0 = sector.walls[i].corner.index;
        var v1 = sector.walls[(i + 1) % sector.walls.length].corner.index;
        if (v1 < v0)
            [v0, v1] = [v1, v0];
        var key = `${v0}|${v1}`;
        if (portalMap.has(key))
        {
            const portalTarget = portalMap.get(key);
            sector.walls[i].portalTo = portalTarget.sector.index;
            sector.walls[i].portalSector = portalTarget.sector;
            portalTarget.sector.walls[portalTarget.wallIndex].portalTo = sector.index;
            portalTarget.sector.walls[portalTarget.wallIndex].portalSector = sector;
        }
        else
        {
            portalMap.set(key, { sector, wallIndex: i });
        }
    }
}

// second pass, flags
for (var sectorName in descr.sectors)
{
    const sector = descr.sectors[sectorName];

    for (var i = 0; i < sector.walls.length; i++)
    {
        const w = sector.walls[i];

        let defContourTop = true;
        let defContourBottom = true;
        if (w.portalTo >= 0)
        {
            defContourTop = sector.yEnd != w.portalSector.yEnd;
            defContourBottom = sector.y != w.portalSector.y;
        }
        
        if (!("contourLeft" in w)) w.contourLeft = true;
        if (!("contourLeftPortal" in w)) w.contourLeftPortal = true;
        if (!("contourRight" in w)) w.contourRight = false;
        if (!("contourRightPortal" in w)) w.contourRightPortal = false;
        if (!("contourTop" in w)) w.contourTop = defContourTop;
        if (!("contourTopPortal" in w)) w.contourTopPortal = defContourTop;
        if (!("contourBottom" in w)) w.contourBottom = defContourBottom;
        if (!("contourBottomPortal" in w)) w.contourBottomPortal = defContourBottom;
        if (!("breakable" in w)) w.breakable = false;

        w.flags = 0 |
            (w.contourLeft          ? (1 << 0) : 0) |
            (w.contourLeftPortal    ? (1 << 1) : 0) |
            (w.contourRight         ? (1 << 2) : 0) |
            (w.contourRightPortal   ? (1 << 3) : 0) |
            (w.contourTop           ? (1 << 4) : 0) |
            (w.contourTopPortal     ? (1 << 5) : 0) |
            (w.contourBottom        ? (1 << 6) : 0) |
            (w.contourBottomPortal  ? (1 << 7) : 0);
    }
}

level.playerStart.position = vecvalue(descr.playerStart.position, "playerStart_position");
level.playerStart.sector = descr.sectors[descr.playerStart.sector.trim()].index;
level.playerStart.height = ("y" in descr.playerStart)
    ? fvalue(descr.playerStart.y, "playerStart_y")
    : level.sectors[level.playerStart.sector].y;
level.playerStart.angle = fvalue(descr.playerStart.angle, "playerStart_angle");

if ("zoom" in descr)
{
    const zoom = fvalue(descr.zoom, "zoom");
    for (var i = 0; i < level.vertices.length; i++)
    {
        level.vertices[i].x *= zoom;
        level.vertices[i].y *= zoom;
    }
    level.playerStart.position.x *= zoom;
    level.playerStart.position.y *= zoom;
}


level.vertexCount = level.vertices.length;
level.sectorCount = level.sectors.length;
level.wallCount = level.walls.length;
fs.writeFileSync("resources/levels/test.bin", FormatLevel.write(level));
