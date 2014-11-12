local class        = require 'middleclass'
local Vec          = require 'core/Vector'
local Quat         = require 'core/Quaternion'
local Controllable = require 'core/Controllable'
local EventSource  = require 'core/EventSource'
local isBetween    = math.isBetween
local boundBy      = math.boundBy


local maxRange = math.pi;


local EgoCameraController = class('core/EgoCameraController')
EgoCameraController:include(Controllable)
EgoCameraController:include(EventSource)


function EgoCameraController:initialize()
    self:initializeEventSource()
    self.rotation = Vec(0, 0)
    self.range = nil
    self.orientation = Quat()
end

function EgoCameraController:destroy()
    self:destroyEventSource()
end

function EgoCameraController:setRange( range )
    assert(Vec:isInstance(range) and
           #range == 2,
           isBetween(range[1], 0, maxRange) and
           isBetween(range[2], 0, maxRange),
           'Must be called with a valid vector.')
    self.range = range
end

function EgoCameraController:getOrientation()
    return self.orientation
end

function EgoCameraController:_rotate( offset )
    local rotation = self.rotation
    rotation = rotation + offset

    local range = self.range
    if range then
        rotation[1] = boundBy(rotation[1], -range[1], range[1])
        rotation[2] = boundBy(rotation[2], -range[2], range[2])
    end

    self.rotation = rotation

    local orientation = Quat(rotation)
    self.orientation = orientation

    self:fireEvent('orientation-updated', orientation)
end

EgoCameraController:mapControl('look-x', function( self, absolute, delta )
    delta = delta * 0.01
    self:_rotate(Vec(0, delta))
end)

EgoCameraController:mapControl('look-y', function( self, absolute, delta )
    delta = delta * 0.01
    self:_rotate(Vec(delta, 0))
end)


return EgoCameraController
