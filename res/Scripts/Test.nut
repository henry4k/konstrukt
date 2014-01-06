print("Squirrels in space!")

map <- require("map")
math <- require("math")

local mapWidth = 20
local mapDepth = 20

local voidTileDef = map.FindTileDefinitionByName("Void")
local tileDefs = [
    map.FindTileDefinitionByName("Wall")
]

map.GenerateMap(mapWidth, mapDepth)
for(local z = 0.0; z < mapDepth; z+=map.TILE_SIZE.z)
for(local x = 0.0; x < mapWidth; x+=map.TILE_SIZE.x)
{
    local tileDef = null
    if(math.Random() >= 0.9)
        tileDef = math.RandomArrayElement(tileDefs)
    else
        tileDef = voidTileDef

    local tile = tileDef.createTile(x,z)
    tile.save()
}
map.UpdateMap()



controls <- require("controls")

testkey <- controls.RegisterKeyControl("testkey", function( pressed )
{
    print("testkey: "+pressed)
})

testaxis <- controls.RegisterAxisControl("testaxis", function( absolute, delta )
{
    print("testaxis: "+absolute+" ("+delta+")")
})
