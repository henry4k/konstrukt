local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local WorldObject = require 'core/world/WorldObject'
local Controlable = require 'core/world/Controlable'


--- Things that the player can take control of.
local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controlable)


function Actor:initialize()
    WorldObject.initialize(self)
end

function Actor:destroy()
    WorldObject.destroy(self)
end

function Actor:activate( pressed )
end

Actor:mapControl('activate', Actor.activate)


return Actor
