local assert = assert
local class = require 'middleclass'
local Object = class.Object
local Actor = require 'core/world/Actor'


--- Provides an actor with input events.
local ActorController = class('core/ActorController')

function ActorController:initialize()
    self.target = nil
end

function ActorController:destroy()
end

function ActorController:setTarget( target )
    assert(Object.isInstanceOf(target, Actor), 'Must be called with an actor.')
    self.target = target
end


return ActorController
