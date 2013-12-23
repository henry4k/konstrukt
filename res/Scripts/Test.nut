local math = require("math")
local map = require("map")

print("Squirrels in space!")

local matA = math.Matrix4()
local matB = math.Matrix4()
local matC = matA*matB
matC = matC.translate(1,2,3)
matC = matC.scale(1,2,3)
matC = matC.rotate(1,2,3)

/*
MapWidth <- 10
MapDepth <- 10

Tiles <- [
    map.CreateTileDefinition("Void"),
    map.CreateTileDefinition("Wall")
]

map.GenerateMap(MapWidth, MapDepth)

for(local z = 0; z < MapDepth; ++z)
for(local x = 0; x < MapWidth; ++x)
{
    map.SetTileAt(x,z, math.RandomTableEntry(Tiles));
}
*/



controls = require("controls")

testkey <- controls.RegisterKeyControl("testkey", function( pressed )
{
    print("testkey: "+pressed)
})

testaxis <- controls.RegisterAxisControl("testaxis", function( absolute, delta )
{
    print("testaxis: "+absolute+" ("+delta+")")
})
