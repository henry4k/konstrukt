print("Squirrels in space!")

MeshBuffer <- require("meshbuffer")
local myMesh = MeshBuffer()
myMesh.appendIndices([0,1,2,3])

myMesh.appendVertices(
    [
    {
        x=42, y=43, z=44,
        r=0.1, g=0.2, b=0.4,
        u=0, v=1,
        nx=0.5, ny=0, nz=1
    },
    {
        x=42, y=43, z=44,
        r=0.1, g=0.2, b=0.4,
        u=0, v=1,
        nx=0.5, ny=0, nz=1
    },
    {
        x=42, y=43, z=44,
        r=0.1, g=0.2, b=0.4,
        u=0, v=1,
        nx=0.5, ny=0, nz=1
    },
    {
        x=42, y=43, z=44,
        r=0.1, g=0.2, b=0.4,
        u=0, v=1,
        nx=0.5, ny=0, nz=1
    }
    ]
)


return

math <- require("math")
coord <- require("coord")
map <- require("map")
tiles <- require("tiles")

require("editor")

local mapWidth = 8
local mapDepth = 8

local spaceDefs = [
    map.FindTileDefinitionByName("Void")
    map.FindTileDefinitionByName("Floor")
    map.FindTileDefinitionByName("Floor")
    map.FindTileDefinitionByName("Floor")
]

local structureDefs = [
    map.FindTileDefinitionByName("Wall")
    map.FindTileDefinitionByName("Door")
    map.FindTileDefinitionByName("Window")
    map.FindTileDefinitionByName("Console")
]

map.GenerateMap(mapWidth, mapDepth)
for(local z = 0.0; z < mapDepth; z+=coord.TILE_SIZE.z)
for(local x = 0.0; x < mapWidth; x+=coord.TILE_SIZE.x)
{
    local tileDef = null
    if(math.Random() >= 0.9)
        tileDef = math.RandomArrayElement(structureDefs)
    else
        tileDef = math.RandomArrayElement(spaceDefs)
    local tile = tileDef.createTile(x,z)
    tile.save()
}

map.UpdateMap()



controls <- require("controls")
effects <- require("effects")


testkey <- controls.RegisterKeyControl("testkey", function( pressed )
{
    if(pressed)
    {
        effects.SetLight(
            {r=0.1, g=0.05, b=0.05},
            {r=0.4, g=0.0, b=0.0},
            {x=0, y=1, z=0}
        )
    }
    else
    {
        effects.SetLight(
            {r=0.1, g=0.1, b=0.1},
            {r=0.9, g=0.9, b=0.9},
            {x=1, y=1, z=1}
        )
    }
})

testaxis <- controls.RegisterAxisControl("testaxis", function( absolute, delta )
{
    print("testaxis: "+absolute+" ("+delta+")")
})
