local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Controlable = require 'core/Controlable'
local WorldObject = require 'core/world/WorldObject'


--- Things that the player can take control of.
local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controlable)


function Actor:initialize()
    WorldObject.initialize(self)
end

function Actor:destroy()
    WorldObject.destroy(self)
end

function Actor:activate( absolute, delta )
    if delta > 0 then
        print(tostring(self)..' activates something!')
    end
end

Actor:mapControl('activate', Actor.activate)


return Actor
