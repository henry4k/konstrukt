map <- require("map")
mesh <- require("mesh")
math <- require("math")


// ----- Direction ------

Direction <- {
    NORTH = 0,
    WEST  = 1,
    SOUTH = 2,
    EAST  = 3
}

function InverseDirection( direction )
{
    switch(direction)
    {
        case Direction.NORTH: return Direction.SOUTH
        case Direction.WEST:  return Direction.EAST
        case Direction.SOUTH: return Direction.NORTH
        case Direction.EAST:  return Direction.WEST
        default: throw "Unknown direction '"+direction+"'"
    }
}

function DirectionToRotation( direction )
{
    switch(direction)
    {
        case Direction.NORTH: return 0.0
        case Direction.WEST:  return 0.5*math.PI
        case Direction.SOUTH: return 1.0*math.PI
        case Direction.EAST:  return 1.5*math.PI
        default: throw "Unknown direction '"+direction+"'"
    }
}

function DirectionToOffset( direction )
{
    switch(direction)
    {
        case Direction.NORTH: return {x=0,z=1}
        case Direction.WEST:  return {x=1,z=0}
        case Direction.SOUTH: return {x=0,z=-1}
        case Direction.EAST:  return {x=-1,z=0}
        default: throw "Unknown direction '"+direction+"'"
    }
}



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
        map.SetTileDefinitionAt(x, z, definition)
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

    function generateStaticSolid( solidBuffer )
    {
        // No collision
    }
}

map.RegisterTileDefinition({
    id = null,
    name = "Void",
    createTile = function( x, z )
    {
        return VoidTile(this, x, z)
    }
})




class FloorTile extends Tile
{
    static floorMesh = mesh.LoadMesh("Meshes/Floor-1x1.ply")

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
        local meshMatrix = math.Matrix4().translate(x,0,z)

        meshBuffer.addMesh(
            floorMesh,
            meshMatrix
        )
    }

    function generateStaticSolid( solidBuffer )
    {
        // Nope.
    }
}

map.RegisterTileDefinition({
    id = null,
    name = "Floor",
    createTile = function( x, z )
    {
        return FloorTile(this, x, z)
    }
})



class WallTile extends Tile
{
    static wallMesh = mesh.LoadMesh("Meshes/Wall-1x1.ply")
    static sideMesh = mesh.LoadMesh("Meshes/WallSide.ply")

    direction = null

    function constructor( definition, x, z )
    {
        base.constructor(definition, x, z)
        direction = math.RandomArrayElement([
            Direction.NORTH,
            Direction.WEST,
            Direction.SOUTH,
            Direction.EAST
        ])
    }

    function load()
    {
        base.load()
        direction = map.GetTileDataAt(x, z, map.DataType.UINT8, 0)
    }

    function save()
    {
        base.save()
        map.SetTileDataAt(x, z, map.DataType.UINT8, 0, direction)
    }

    function coversDirection( direction )
    {
        return true // Cause we cover *all* directions. :)
    }

    function generateStaticMesh( meshBuffer )
    {
        local rotation = DirectionToRotation(direction)
        local meshMatrix = math.Matrix4().translate(x,0,z).rotate(rotation, 0,1,0)

        meshBuffer.addMesh(
            wallMesh,
            meshMatrix
        )

        local offset = DirectionToOffset(direction)

        //if(IsTileSideCovered(this, direction) == false)
        //{
            meshBuffer.addMesh(
                sideMesh,
                meshMatrix
            )
        //}

        //if(IsTileSideCovered(this, InverseDirection(direction)) == false)
        //{
            meshBuffer.addMesh(
                sideMesh,
                math.Matrix4(meshMatrix).rotate(math.PI,0,1,0)
            )
        //}
    }

    function generateStaticSolid( solidBuffer )
    {
        solidBuffer.addAabb({
            position =
            {
                x = x,
                y = map.TILE_SIZE.y / 2.0,
                z = z
            },
            halfWidth =
            {
                x = map.TILE_SIZE.x / 2.0,
                y = map.TILE_SIZE.y / 2.0,
                z = map.TILE_SIZE.z / 2.0
            }
        })
    }
}

map.RegisterTileDefinition({
    id = null,
    name = "Wall",
    createTile = function( x, z )
    {
        return WallTile(this, x, z)
    }
})




class DoorTile extends Tile
{
    static doorMesh = mesh.LoadMesh("Meshes/Door.ply")

    direction = null

    function constructor( definition, x, z )
    {
        base.constructor(definition, x, z)
        direction = math.RandomArrayElement([
            Direction.NORTH,
            Direction.WEST,
            Direction.SOUTH,
            Direction.EAST
        ])
    }

    function load()
    {
        base.load()
        direction = map.GetTileDataAt(x, z, map.DataType.UINT8, 0)
    }

    function save()
    {
        base.save()
        map.SetTileDataAt(x, z, map.DataType.UINT8, 0, direction)
    }

    /*
       Maybe just use tile physics information for determinging if a tile is solid (i.e. if gas can flow through it).
       Because implementing coversDirection correctly seems to be pretty complex to me.
    */
    function coversDirection( direction )
    {
        if(this.rotation * NORTH == direction)
            return true
        else
            return false

        rotated = Rotate({x=1,z=0}, this.rotation)
        rotated = {x=0,z=1}

        return true
    }

    function generateStaticMesh( meshBuffer )
    {
        local rotation = DirectionToRotation(direction)
        local meshMatrix = math.Matrix4().translate(x,0,z).rotate(rotation, 0,1,0)

        meshBuffer.addMesh(
            doorMesh,
            meshMatrix
        )
    }

    function generateStaticSolid( solidBuffer )
    {
        local rotation = DirectionToRotation(direction)
        local solidMatrix = math.Matrix4().translate(x,0,z).rotate(rotation, 0,1,0)

        solidBuffer.addAabb(
            {
                position =
                {
                    x = 0,
                    y = map.TILE_SIZE.y / 2.0,
                    z = -0.25
                },
                halfWidth =
                {
                    x = map.TILE_SIZE.x / 2.0,
                    y = map.TILE_SIZE.y / 2.0,
                    z = (map.TILE_SIZE.z / 2.0) * 2.0
                }
            },
            solidMatrix
        )
    }
}

map.RegisterTileDefinition({
    id = null,
    name = "Door",
    createTile = function( x, z )
    {
        return DoorTile(this, x, z)
    }
})


return {
    Direction = Direction,
    DirectionToRotation = DirectionToRotation,
    DirectionToOffset = DirectionToOffset
}
