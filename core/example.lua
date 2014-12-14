--- @module example

example = {}

---
-- @type Player
example.Player = class()

---
-- @type Client
example.Client = class()

---
-- @type Vector
example.Vector = class()

--- Spawns a new player.
--
-- Clients that have no player can spawn one, calling
-- this weird function.
--
-- @param[type=Client] owner
-- Client that owns the new player.
--
-- @param[type=string,opt='newb'] name
-- The players name.
--
-- @param[type=Vector,opt] position
-- @{Player} spawns at the given position.
-- Defaults to `Vec(0,0,0)`.
--
-- @return[type=Player]
-- The newly created player or `nil` if something went wrong.
--
-- @usage
-- igor = SpawnPlayer(mahclient, 'Igor', peter:getPosition)
-- igor:jump()
--
function example.SpawnPlayer( owner, name, position )
end


return example
