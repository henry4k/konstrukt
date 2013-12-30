math <- require("math")
map  <- require("map")
mesh <- require("mesh")

print("Squirrels in space!")

MapWidth <- 10
MapDepth <- 10

WallMesh <- mesh.LoadMesh("Meshes/Wall-1x1.ply")

Tiles <- [
    map.CreateTileDefinition("Void", null, null),
    map.CreateTileDefinition("Wall",
        function( x, z, meshBuffer ) // staticTileMeshFn
        {
            meshBuffer.addMesh(
                WallMesh,
                math.Matrix4().translate(x,0,z).rotate(math.PI/2.0,0,1,0)
            )
        },
        null // staticTileSolidFn
    )
]

map.GenerateMap(MapWidth, MapDepth)

for(local z = 0.0; z < MapDepth; z+=map.TILE_SIZE.z)
for(local x = 0.0; x < MapWidth; x+=map.TILE_SIZE.x)
{
    map.SetTileAt(x,z, 0) //math.RandomTableEntry(Tiles))
}
map.SetTileAt(1.0, 0.0, 1)
map.SetTileAt(1.5, 0.0, 1)
map.SetTileAt(2.0, 0.0, 1)
map.SetTileAt(2.5, 0.0, 1)
map.SetTileAt(3.0, 0.0, 1)
map.UpdateMap()



controls = require("controls")

testkey <- controls.RegisterKeyControl("testkey", function( pressed )
{
    print("testkey: "+pressed)
})

testaxis <- controls.RegisterAxisControl("testaxis", function( absolute, delta )
{
    print("testaxis: "+absolute+" ("+delta+")")
})
