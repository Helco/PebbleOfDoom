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
    .int32LE("entityCount")
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
    .uint8("flags");

const FormatEntity = new Format()
    .nest("location", FormatLocation)
    .int32LE("sprite")
    .uint8("type")
    .uint8("arg1")
    .uint8("arg2")
    .uint8("arg3");

const FormatLevel = new Format()
    .int32LE("storageVersion")
    .int32LE("sectorCount")
    .int32LE("wallCount")
    .int32LE("vertexCount")
    .int32LE("entityCount")
    .nest("playerStart", FormatLocation)
    .listEof("vertices", FormatVector)
    .listEof("sectors", FormatSector)
    .listEof("walls", FormatWall)
    .listEof("entities", FormatEntity);

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
    ],
    entities: [

    ]
};

const levelName = "overworld";
const descr = yaml.parse(fs.readFileSync("resources/levels/" + levelName + ".yaml", "utf-8"));
level.vertices = [];
level.sectors = [];
level.walls = [];
level.entities = [];

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

if (!("vertices" in descr))
    descr.vertices = {};

if ("vertexRects" in descr)
{
    for (var v in descr.vertexRects)
    {
        const origin = vecvalue(descr.vertexRects[v].origin, v + "_origin");
        const size = vecvalue(descr.vertexRects[v].size, v + "_size");
        descr.vertices[v + "LL"] = `${origin.x}, ${origin.y}`;
        descr.vertices[v + "RL"] = `${origin.x+size.x}, ${origin.y}`;
        descr.vertices[v + "LH"] = `${origin.x}, ${origin.y+size.y}`;
        descr.vertices[v + "RH"] = `${origin.x+size.x}, ${origin.y+size.y}`;
    }
}

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

function edgeKey(v0, v1)
{
    if (typeof v0 !== "number")
        v0 = v0.index;
    if (typeof v1 !== "number")
        v1 = v1.index;
    if (v1 < v0)
        [v0, v1] = [v1, v0];
    return `${v0}|${v1}`;
}

const defaultY = ("defaultY" in descr)
    ? fvalue(descr.defaultY, "defaultY")
    : null;
const defaultH = ("defaultH" in descr)
    ? fvalue(descr.defaultH, "defaultH")
    : null;

// First pass
for (var sectorName in descr.sectors)
{
    const sector = descr.sectors[sectorName];
    if ("h" in sector)
        sector.height = sector.h = fvalue(sector.h, sectorName + "_h");
    else if (defaultH !== null)
        sector.height = sector.h = defaultH;
    else
        throw sectorName + " needs h";
    if ("y" in sector)
        sector.heightOffset = sector.y = fvalue(sector.y, sectorName + "_y");
    else if (defaultY !== null)
        sector.heightOffset = sector.y = defaultY;
    else
        throw sectorName + " needs y";
    sector.yEnd = sector.y + sector.h;
    sector.floorColor = ("floorColor" in sector) && sector["floorColor"] !== undefined
        ? colvalue(sector.floorColor, sectorName + "_floorColor")
        : defaultFloorColor;
    sector.ceilColor = ("ceilColor" in sector) && sector["ceilColor"] !== undefined
        ? colvalue(sector.ceilColor, sectorName + "_ceilColor")
        : defaultCeilColor;

    if (!("entities" in sector))
        sector.entities = [];

    sector.wallOffset = level.walls.length;
    sector.wallCount = sector.walls.length;
    sector.entityCount = sector.entities.length;
    sector.index = level.sectors.length;
    level.sectors.push(sector);

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
        var key = edgeKey(
            sector.walls[i].corner,
            sector.walls[(i + 1) % sector.walls.length].corner);
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

    for (var i = 0; i < sector.entities.length; i++)
    {
        var e = sector.entities[i];

        e.location = { 
            sector: sector.index,
            angle: 0,
            height: sector.y,
            position: vecvalue(e.position, `${sectorName}_entity${i}_pos`)
        };
        if ("y" in e)
            e.location.height += fvalue(e.y, `${sectorName}_entity${i}_y`);
        else
            e.location.height += 0;
        if ("angle" in e)
            e.location.angle = fvalue(e.angle, `${sectorName}_entity${i}_y`);
        e.sprite = ivalue(e.sprite, `${sectorName}_entity${i}_sprite`);
        function optivalue(obj, prop, def, name) {
            return (prop in obj) ? ivalue(obj[prop], `${name}_${prop}`) : def;
        }
        e.type = optivalue(e, "type", 0, `${sectorName}_entity${i}`);
        e.arg1 = optivalue(e, "arg1", 0, `${sectorName}_entity${i}`);
        e.arg2 = optivalue(e, "arg2", 0, `${sectorName}_entity${i}`);
        e.arg3 = optivalue(e, "arg3", 0, `${sectorName}_entity${i}`);
        e.index = level.entities.length;

        level.entities.push(e);
    }
}

const disabledContours = new Set();
if ("disabledContours" in descr)
{
    for (var i = 0; i < descr.disabledContours.length; i++)
    {
        if (!(descr.disabledContours[i] in descr.vertices))
            throw "Invalid vertex for disabled contours: " + descr.disabledContours[i];
        disabledContours.add(descr.vertices[descr.disabledContours[i]].index);
    }
}

const disabledPortalContours = new Set();
if ("disabledPortalContours" in descr)
{
    for (var i = 0; i < descr.disabledPortalContours.length; i++)
    {
        if (!(descr.disabledPortalContours[i] in descr.vertices))
            throw "Invalid vertex for disabled portal contours: " + descr.disabledPortalContours[i];
        disabledPortalContours.add(descr.vertices[descr.disabledPortalContours[i]].index);
    }
}

const enabledBottomContours = new Set();
if ("enabledBottomContours" in descr)
{
    for (var i = 0; i < descr.enabledBottomContours.length; i++)
    {
        var parts = descr.enabledBottomContours[i].split(",").map(p => p.trim()).filter(p => p !== "");
        if (parts.length != 2)
            throw "Invalid part count for enabled bottom contours: " + descr.enabledBottomContours[i];
        if (!(parts[0] in descr.vertices) || !(parts[1] in descr.vertices))
            throw "Invalid vertices for enabled bottom contours: " + descr.enabledBottomContours[i];

        enabledBottomContours.add(edgeKey(
            descr.vertices[parts[0]],
            descr.vertices[parts[1]]
        ));
    }
}

// second pass, flags
for (var sectorName in descr.sectors)
{
    const sector = descr.sectors[sectorName];

    for (var i = 0; i < sector.walls.length; i++)
    {
        const w = sector.walls[i];
        const myRightV = w.startCorner;
        const myLeftV = sector.walls[(i + 1) % sector.walls.length].startCorner;
        const key = edgeKey(myRightV, myLeftV);

        let defContourLeft = true;
        let defContourLeftPortal = true;
        let defContourRight = false;
        let defContourRightPortal = false;
        let defContourTop = true;
        let defContourBottom = true;
        if (w.portalTo >= 0)
        {
            defContourTop = sector.yEnd != w.portalSector.yEnd;
            defContourBottom = sector.y != w.portalSector.y;
        }

        if ("defaultContourTop" in descr) defContourTop = descr.defaultContourTop;

        if ("contourLeft" in sector) defContourLeft = sector.contourLeft;
        if ("contourLeftPortal" in sector) defContourLeftPortal = sector.contourLeftPortal;
        if ("contourRight" in sector) defContourRight = sector.contourRight;
        if ("contourRightPortal" in sector) defContourRightPortal = sector.contourRightPortal;

        if (disabledContours.has(myRightV)) { defContourRight = false; defContourRightPortal = false; }
        if (disabledContours.has(myLeftV)) { defContourLeft = false; defContourLeftPortal = false; }
        if (disabledPortalContours.has(myRightV)) defContourRightPortal = false;
        if (disabledPortalContours.has(myLeftV)) defContourLeftPortal = false;
        if (enabledBottomContours.has(key)) defContourBottom = true;
        
        if (!("contourLeft" in w)) w.contourLeft = defContourLeft;
        if (!("contourLeftPortal" in w)) w.contourLeftPortal = defContourLeftPortal;
        if (!("contourRight" in w)) w.contourRight = defContourRight;
        if (!("contourRightPortal" in w)) w.contourRightPortal = defContourRightPortal;
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
    for (var i = 0; i < level.entities.length; i++)
    {
        level.entities[i].location.position.x *= zoom;
        level.entities[i].location.position.y *= zoom;
    }
    level.playerStart.position.x *= zoom;
    level.playerStart.position.y *= zoom;
}


level.vertexCount = level.vertices.length;
level.sectorCount = level.sectors.length;
level.wallCount = level.walls.length;
level.entityCount = level.entities.length;
fs.writeFileSync("resources/levels/" + levelName + ".bin", FormatLevel.write(level));
