coord <- require("coord")
MeshBuffer <- require("meshbuffer")
SolidBuffer <- require("solidbuffer")

TileNameToDefinition <- {}
TileIdToDefinition   <- {}

/*
tile definition:
{
    id = 42
    name = "foo"
    daoConstructor = function( x, z ) {...}
}

tile dao:
{
    tileDefinition = ...
    origin = { x=4, z=10 }

    ...
}
*/

REFERENCE_TILE_ID <- null

function IsTileDefinition( d )
{
    return (
        "id" in d &&
        "name" in d &&
        "createTile" in d
    )
}

function IsTile( t )
{
    return (
        "definition" in t &&
        "x" in t &&
        "z" in t &&
        "load" in t &&
        "save" in t &&
        "generateStaticMesh" in t &&
        "generateStaticSolid" in t
    )
}

function RegisterTileDefinition( definition )
{
    if(!IsTileDefinition(definition))
        throw "Not a tile definition"
    if(definition.name in TileNameToDefinition)
        throw "Duplicate tile with name '"+definition.name+"'"
    definition.id <- ::native.CreateTileDefinition(definition.name)
    TileNameToDefinition[definition.name] <- definition
    TileIdToDefinition[definition.id] <- definition
}

function FindTileDefinitionByName( name )
{
    if(name in TileNameToDefinition)
        return TileNameToDefinition[name]
    else
        return null
}

function OnGenerateStaticTileMesh( id, x, z, meshBufferHandle )
{
    local worldCoord = coord.MapToWorldCoord({x=x,z=z})
    local definition = TileIdToDefinition[id]
    local tile = definition.createTile(worldCoord.x, worldCoord.z)
    tile.load()
    tile.generateStaticMesh(MeshBuffer(meshBufferHandle))
}
::native.RegisterStaticTileMeshGeneratorCallback(OnGenerateStaticTileMesh)

function OnGenerateStaticTileSolid( id, x, z )
{
    local worldCoord = coord.MapToWorldCoord({x=x,z=z})
    local definition = TileIdToDefinition[id]
    local tile = definition.createTile(worldCoord.x, worldCoord.z)
    tile.load()
    local solidBuffer = SolidBuffer()
    tile.generateStaticSolid(solidBuffer)
    return solidBuffer.handle
}
::native.RegisterStaticTileSolidGeneratorCallback(OnGenerateStaticTileSolid)

_MapSize <- {x=0, z=0}
function GenerateMap( width, depth )
{
    local mapCoord = coord.WorldToMapCoord({x=width, z=depth})
    ::native.GenerateMap(
        mapCoord.x,
        mapCoord.z
    )
    _MapSize = mapCoord
}

function GetTileAt( x, z )
{
    local mapCoord = coord.WorldToMapCoord({x=x, z=z})
    print("x="+mapCoord.x+" z="+mapCoord.z)
    print("mapx="+_MapSize.x+" mapz="+_MapSize.z)
    if(mapCoord.x < 0 || mapCoord.z < 0 || mapCoord.x >= _MapSize.x || mapCoord.z >= _MapSize.z)
        return null // Weil out of range
    local id = ::native.GetTileDefinitionAt(
        mapCoord.x,
        mapCoord.z
    )
    local definition = TileIdToDefinition[id]
    local tile = definition.createTile(x,z)
    tile.load()
    return tile
}

function SetTileDefinitionAt( x, z, definition )
{
    local mapCoord = coord.WorldToMapCoord({x=x, z=z})
    ::native.SetTileAt(
        mapCoord.x,
        mapCoord.z,
        definition.id
    )
}

DataType <- {
    UINT8  = 0,
    INT8   = 1,
    UINT16 = 2,
    INT16  = 3,
    UINT32 = 4,
    INT32  = 5,
    FLOAT32 = 6
}

function GetTileDataAt( x, z, position, type )
{
    local mapCoord = coord.WorldToMapCoord({x=x,z=z})
    return ::native.GetTileDataAt(
        mapCoord.x,
        mapCoord.z
        position,
        type
    )
}

function SetTileDataAt( x, z, position, type, value )
{
    local mapCoord = coord.WorldToMapCoord({x=x,z=z})
    ::native.SetTileDataAt(
        mapCoord.x,
        mapCoord.z
        position,
        type,
        value
    )
}

RegisterTileDefinition({
    id = null,
    name = "Reference",
    createTile = function( x, z )
    {
        local xoffset = GetTileDataAt(x, z, 0, DataType.INT8) // x offset -127 to +127
        local zoffset = GetTileDataAt(x, z, 1, DataType.INT8) // z offset -127 to +127
        local definition = GetTileDefinitionAt(x+xoffset, z+zoffset)
        if(definition.name != "Reference")
            throw "Reference tile points to another reference!"
        return definition.createTile(x+xoffset, z+zoffset)
    }
})

return {
    IsTileDefinition = IsTileDefinition,
    IsTile = IsTile,
    RegisterTileDefinition = RegisterTileDefinition,
    FindTileDefinitionByName = FindTileDefinitionByName,
    GenerateMap = GenerateMap,
    UpdateMap = ::native.UpdateMap,
    GetTileAt = GetTileAt,
    SetTileDefinitionAt = SetTileDefinitionAt,
    DataType = DataType,
    GetTileDataAt = GetTileDataAt,
    SetTileDataAt = SetTileDataAt
}
