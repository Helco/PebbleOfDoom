# Level format

## Level

|   Type   | Name | Comments |
|:--------:|:----:|----------|
| int      | sectorCount | |
| Sector[] | sectors | |

## Sector

A sector is a convex polygon whose edges represent walls.

|   Type    | Name | Comments |
|:---------:|:----:|----------|
| int       | wallCount | |
| int       | height | |
| int       | heightOffset | |
| TextureId | floorTexture | |
| Mat3x2    | floorMatrix | used for texture-mapping |
| TextureId | ceilTexture | |
| Mat3x2    | ceilMatrix | used for texture-mapping |
| Wall[]    | walls | |

## Wall

A wall is a vertical section which can contain a portal, in which case it has up to three sections:

- Lower wall
- Portal through which another sector can be seen
- Upper wall

|   Type    | Name | Comments |
|:---------:|:----:|----------|
| xz        | startCorner | endCorner is `walls[(index + 1) % wallCount].startCorner]` |
| TextureId | texture | |
| TexCoord  | texCoord | |
| int       | portalStart | |
| int       | portalHeight | |
| int       | portalTarget | index of target sector, `-1` if no portal |

## TexCoord

| Type | Name  | Comments |
|:----:|:-----:|----------|
| xy   | start | |
| xy   | end   | |
