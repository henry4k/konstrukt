print("Squirrels in space!")

math <- require("math")
map <- require("map")
tiles <- require("tiles")

local mapWidth = 20
local mapDepth = 20

local spaceDefs = [
    //map.FindTileDefinitionByName("Void"),
    map.FindTileDefinitionByName("Floor"),
    map.FindTileDefinitionByName("Floor"),
    map.FindTileDefinitionByName("Floor")
]

local structureDefs = [
    map.FindTileDefinitionByName("Wall")
]

map.GenerateMap(mapWidth, mapDepth)
for(local z = 0.0; z < mapDepth; z+=map.TILE_SIZE.z)
for(local x = 0.0; x < mapWidth; x+=map.TILE_SIZE.x)
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

testkey <- controls.RegisterKeyControl("testkey", function( pressed )
{
    print("testkey: "+pressed)
})

testaxis <- controls.RegisterAxisControl("testaxis", function( absolute, delta )
{
    print("testaxis: "+absolute+" ("+delta+")")
})
