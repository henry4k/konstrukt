TILE_SIZE <- {
    x = 0.5,
    y = 2.0,
    z = 0.5
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

return {
    TILE_SIZE = TILE_SIZE,
    WorldToMapCoord = WorldToMapCoord,
    MapToWorldCoord = MapToWorldCoord
}
