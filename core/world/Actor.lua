local assert = assert
local class = require 'middleclass'
local Object = class.Object
local WorldObject = require 'core/world/WorldObject'
local Controlable = require 'core/world/Controlable'


--- Things that the player can take control of.
local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controlable)

function Actor:initialize()
    WorldObject.initialize(self)
    Controlable.initializeControlable(self)
end

function Actor:destroy()
    Controlable.destroyControlable(self)
    WorldObject.destroy(self)
end


return Actor
