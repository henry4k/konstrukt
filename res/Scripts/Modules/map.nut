MeshBuffer = require("meshbuffer")

Callbacks <- {}

TILE_SIZE <- {
    "x": 0.5,
    "y": 2.0,
    "z": 0.5
}

function CreateTileDefinition( name, staticTileMeshFn, staticTileSolidFn )
{
    local definition = ::native.CreateTileDefinition(name)
    Callbacks[definition] <- {
        "staticTileMeshFn": staticTileMeshFn,
        "staticTileSolidFn": staticTileSolidFn
    }
    return definition
}

function OnGenerateStaticTileMesh( definition, x, z, meshBuffer )
{
    local callbacks = Callbacks[definition]
    if(callbacks.staticTileMeshFn)
    {
        return callbacks.staticTileMeshFn(
            x*TILE_SIZE.x,
            z*TILE_SIZE.z,
            MeshBuffer(meshBuffer)
        )
    }
}
::native.RegisterStaticTileMeshGeneratorCallback(OnGenerateStaticTileMesh)

function OnGenerateStaticTileSolid( definition, x, z )
{
    local callbacks = Callbacks[definition]
    if(callbacks.staticTileSolidFn)
    {
        return callbacks.staticTileSolidFn(
            x*TILE_SIZE.x,
            z*TILE_SIZE.z
        )
    }
}
::native.RegisterStaticTileSolidGeneratorCallback(OnGenerateStaticTileSolid)

function GenerateMap( width, depth )
{
    ::native.GenerateMap(
        (width/TILE_SIZE.x).tointeger(),
        (depth/TILE_SIZE.z).tointeger()
    )
}

function GetTileDefinitionAt( x, z )
{
    ::native.GetTileDefinitionAt(
        (x/TILE_SIZE.x).tointeger(),
        (z/TILE_SIZE.z).tointeger()
    )
}

function SetTileAt( x, z, defintion )
{
    ::native.SetTileAt(
        (x/TILE_SIZE.x).tointeger(),
        (z/TILE_SIZE.z).tointeger(),
        defintion
    )
}

return {
    "TILE_SIZE": TILE_SIZE,
    "CreateTileDefinition": CreateTileDefinition,
    "GenerateMap": GenerateMap,
    "UpdateMap": ::native.UpdateMap,
    "GetTileDefinitionAt": GetTileDefinitionAt,
    "SetTileAt": SetTileAt
}
