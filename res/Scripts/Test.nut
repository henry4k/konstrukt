print("Squirrels in space!")

math <- require("math")
coord <- require("coord")
map <- require("map")
tiles <- require("tiles")

require("editor")

local mapWidth = 4
local mapDepth = 4

local spaceDefs = [
    map.FindTileDefinitionByName("Void")
    map.FindTileDefinitionByName("Floor")
    map.FindTileDefinitionByName("Floor")
    map.FindTileDefinitionByName("Floor")
]

local structureDefs = [
    map.FindTileDefinitionByName("Wall")
    map.FindTileDefinitionByName("Door")
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
