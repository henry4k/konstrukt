map <- require("map")
coord <- require("coord")
controls <- require("controls")

Tiles = []
Selection = 0

function GetPlayerPosition()
{
    local position = ::native.GetPlayerPosition();
    return {
        x = position[0],
        y = position[1],
        z = position[2]
    }
}

function GetPlayerOrientation()
{
    local orientation = ::native.GetPlayerOrientation();
    return {
        x = orientation[0],
        y = orientation[1],
        z = orientation[2]
    }
}

function RayTestMap( rayOrigin, rayDirection )
{
    local blob = ::native.RayTestMap(
        rayOrigin.x,
        rayOrigin.y,
        rayOrigin.z,
        rayDirection.x,
        rayDirection.y,
        rayDirection.z
    );

    local result = {
        length = blob.readn('f'),
        tilePosition = coord.MapToWorldCoord({
            x = blob.readn('i'),
            z = blob.readn('i'),
        })
        solidUserData = blob.readn('i')
    }

    return result
}

function GetTilePositionUnderCursor()
{
    local result = RayTestMap(GetPlayerPosition(), GetPlayerOrientation())
    if(result.length > 10)
        return null
    else
        return result.tilePosition
}

function AddTile( newTile )
{
    assert(map.IsTile(newTile))

    foreach(index,tile in Tiles)
    {
        if(tile.definition == newTile.definition)
        {
            print("Tile '"+tile.definition.name+"' already exists at index "+index)
            Select(index)
            return
        }
    }

    newTile = clone newTile
    if(Selection > Tiles.len()-1)
    {
        Tiles.append(newTile)
        Select(Selection)
    }
    else
    {
        Tiles.insert(Selection+1, newTile)
        Select(Selection+1)
    }
}

function GetSelectedTile()
{
    assert(Tiles.len() > 0)
    return Tiles[Selection]
}

function Select( tileIndex )
{
    assert(Tiles.len() > 0)
    assert(tileIndex >= 0)
    assert(tileIndex < Tiles.len())
    Selection = tileIndex
    print("Selected ["+(Selection+1)+"/"+Tiles.len()+"]: '"+GetSelectedTile().definition.name+"'")
}

function CycleSelection( direction )
{
    local selection = Selection

    if(direction == 1)
    {
        selection++
        if(selection >= Tiles.len())
            selection = 0
    }
    else if(direction == -1)
    {
        selection--
        if(selection < 0)
            selection = Tiles.len()-1
    }

    Select(selection)
}

CycleTileUp <- controls.RegisterKeyControl("cycle-tile-up", function( pressed )
{
    if(pressed)
        CycleSelection(1)
})

CycleTileDown <- controls.RegisterKeyControl("cycle-tile-down", function( pressed )
{
    if(pressed)
        CycleSelection(-1)
})

CycleTile <- controls.RegisterAxisControl("cycle-tile", function( absolute, delta )
{
    if(delta < 0)
        CycleSelection(-1)
    else if(delta > 0)
        CycleSelection(1)
})

CopyTile <- controls.RegisterKeyControl("copy-tile", function( pressed )
{
    if(pressed)
    {
        local tilePosition = GetTilePositionUnderCursor()
        if(tilePosition)
            AddTile(map.GetTileAt(tilePosition.x, tilePosition.z))
        else
            print("No hit.")
    }
})

PlaceTile <- controls.RegisterKeyControl("place-tile", function( pressed )
{
    if(pressed)
    {
        local tilePosition = GetTilePositionUnderCursor()
        if(tilePosition)
        {
            local tile = GetSelectedTile()
            tile.x = tilePosition.x
            tile.z = tilePosition.z
            tile.save()
            map.UpdateMap()
        }
        else
            print("No hit.")
    }
})

RotateTile <- controls.RegisterKeyControl("rotate-tile", function( pressed )
{
    if(pressed)
    {
        local tilePosition = GetTilePositionUnderCursor()
        if(tilePosition)
        {
            local tile = map.GetTileAt(tilePosition.x, tilePosition.z)
            if("direction" in tile)
            {
                tile.direction++
                if(tile.direction >= 4)
                    tile.direction = 0
                // TODO: This is messy :(
            }
            tile.save()
            map.UpdateMap()
        }
        else
            print("No hit.")
    }
})


return {
    AddTile = AddTile
}
