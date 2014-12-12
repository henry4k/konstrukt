---
-- @module core.EgoCameraController
-- @includes Controllable EventSource


local class        = require 'middleclass'
local Vec          = require 'core/Vector'
local Quat         = require 'core/Quaternion'
local Controllable = require 'core/Controllable'
local EventSource  = require 'core/EventSource'
local isBetween    = math.isBetween
local boundBy      = math.boundBy
local abs          = math.abs
local sign         = math.sign


local turn = math.pi * 2


local EgoCameraController = class('core/EgoCameraController')
EgoCameraController:include(Controllable)
EgoCameraController:include(EventSource)


function EgoCameraController:initialize()
    self:initializeEventSource()
    self.rotation = Vec(0, 0)
    self.orientation = Quat()
    self:setMaxOffset(nil, turn/4)
end

function EgoCameraController:destroy()
    self:destroyEventSource()
end

function EgoCameraController:setMaxOffset( maxXOffset, maxYOffset )
    assert((maxXOffset == nil or isBetween(maxXOffset, 0, turn/2)) and
           (maxYOffset == nil or isBetween(maxYOffset, 0, turn/2)),
           'Maximum must be an positive value below or equal to a full turn.')
    self.maxXOffset = maxXOffset
    self.maxXOffset = maxYOffset
end

function EgoCameraController:getOrientation()
    return self.orientation
end

function EgoCameraController:_rotate( offset )
    local rotation = self.rotation
    rotation = rotation + offset

    local maxXOffset = self.maxXOffset
    local maxYOffset = self.maxYOffset

    if maxXOffset then
        rotation[1] = boundBy(rotation[1], -maxXOffset, maxXOffset)
    else
        -- wrap rotation
        if abs(rotation[1]) > turn/2 then
            rotation[1] = rotation[1] - sign(rotation[1])*turn
        end
    end

    if maxYOffset then
        rotation[2] = boundBy(rotation[2], -maxYOffset, maxYOffset)
    else
        -- wrap rotation
        if abs(rotation[2]) > turn/2 then
            rotation[2] = rotation[2] - sign(rotation[2])*turn
        end
    end

    self.rotation = rotation

    local orientation = Quat(Vec(rotation[1], 0, 0)) *
                        Quat(Vec(0, rotation[2], 0))
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
