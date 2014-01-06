MeshBuffer <- require("meshbuffer")
mesh <- require("mesh")
math <- require("math")

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

TILE_SIZE <- {
    x = 0.5,
    y = 2.0,
    z = 0.5
}

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

function WorldToMapCoord( w )
{
    return {
        x = (w.x / TILE_SIZE.x).tointeger(),
        z = (w.z / TILE_SIZE.z).tointeger()
    }
}

function MapToWorldCoord( w )
{
    return {
        x = w.x * TILE_SIZE.x,
        z = w.z * TILE_SIZE.z
    }
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
    local worldCoord = MapToWorldCoord({x=x,z=z})
    local definition = TileIdToDefinition[id]
    local tile = definition.createTile(worldCoord.x, worldCoord.z)
    tile.load()
    return tile.generateStaticMesh(MeshBuffer(meshBufferHandle))
}
::native.RegisterStaticTileMeshGeneratorCallback(OnGenerateStaticTileMesh)

function OnGenerateStaticTileSolid( id, x, z )
{
    local worldCoord = MapToWorldCoord({x=x,z=z})
    local definition = TileIdToDefinition[id]
    local tile = definition.createTile(worldCoord.x, worldCoord.z)
    tile.load()
    return tile.generateStaticSolid()
}
::native.RegisterStaticTileSolidGeneratorCallback(OnGenerateStaticTileSolid)

function GenerateMap( width, depth )
{
    local mapCoord = WorldToMapCoord({x=width, z=depth})
    ::native.GenerateMap(
        mapCoord.x,
        mapCoord.z
    )
}

function GetTileAt( x, z )
{
    local mapCoord = WorldToMapCoord({x=x, z=z})
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
    local mapCoord = WorldToMapCoord({x=x, z=z})
    ::native.SetTileAt(
        mapCoord.x,
        mapCoord.z,
        definition.id
    )
}

enum DataType
{
    UINT8,
    INT8,
    UINT16,
    INT16,
    UINT32,
    INT32,
    FLOAT32
}

function GetTileDataAt( x, z, position, type )
{
    return ::native.GetTileDataAt(
        (x/TILE_SIZE.x).tointeger(),
        (z/TILE_SIZE.z).tointeger(),
        position,
        type
    )
}

function SetTileDataAt( x, z, position, type, value )
{
    ::native.SetTileDataAt(
        (x/TILE_SIZE.x).tointeger(),
        (z/TILE_SIZE.z).tointeger(),
        position,
        type,
        value
    )
}

// ---- Tiles ----

class Tile
{
    definition = null
    x = null
    z = null

    function constructor( definition, x, z )
    {
        this.definition = definition
        this.x = x
        this.z = z
    }

    function load()
    {
    }

    function save()
    {
        SetTileDefinitionAt(x, z, definition)
    }
}

class VoidTile extends Tile
{
    function constructor( definition, x, z )
    {
        base.constructor(definition, x, z)
    }

    function load()
    {
        base.load()
    }

    function save()
    {
        base.save()
    }

    function generateStaticMesh( meshBuffer )
    {
        // Nothing here - what have you expected?
    }

    function generateStaticSolid()
    {
        // Dito
    }
}

RegisterTileDefinition({
    id = null,
    name = "Void",
    createTile = function( x, z )
    {
        return VoidTile(this, x, z)
    }
})

enum Rotation
{
    NORTH,
    WEST,
    SOUTH,
    EAST
}

class WallTile extends Tile
{
    static wallMesh = mesh.LoadMesh("Meshes/Wall-1x1.ply")
    static sideMesh = mesh.LoadMesh("Meshes/WallSide.ply")

    rotation = null

    function constructor( definition, x, z )
    {
        base.constructor(definition, x, z)
        rotation = Rotation.NORTH
    }

    function load()
    {
        base.load()
        rotation = GetTileDataAt(x, z, DataType.UINT8, 0)
    }

    function save()
    {
        base.save()
        SetTileDataAt(x, z, DataType.UINT8, 0, rotation)
    }

    function generateStaticMesh( meshBuffer )
    {
        meshBuffer.addMesh(
            wallMesh,
            math.Matrix4().translate(x,0,z)
        )

        meshBuffer.addMesh(
            sideMesh,
            math.Matrix4().translate(x,0,z)
        )

        meshBuffer.addMesh(
            sideMesh,
            math.Matrix4().translate(x,0,z).rotate(math.PI,0,1,0)
        )
    }

    function generateStaticSolid()
    {
        throw "Not implemented"
    }
}

RegisterTileDefinition({
    id = null,
    name = "Wall",
    createTile = function( x, z )
    {
        return WallTile(this, x, z)
    }
})


return {
    TILE_SIZE = TILE_SIZE,
    RegisterTileDefinition = RegisterTileDefinition,
    FindTileDefinitionByName = FindTileDefinitionByName,
    GenerateMap = GenerateMap,
    UpdateMap = ::native.UpdateMap,
    GetTileAt = GetTileAt,
    SetTileDefinitionAt = SetTileDefinitionAt,
    //DataType = DataType,
    GetTileDataAt = GetTileDataAt,
    SetTileDataAt = SetTileDataAt,
    //Rotation = Rotation
}
