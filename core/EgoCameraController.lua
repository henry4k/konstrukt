local class  = require 'middleclass'
local rad    = math.rad
local Vec         = require 'core/Vector'
local Quat        = require 'core/Quaternion'
local Controllable = require 'core/Controllable'
local EventSource = require 'core/EventSource'


local EgoCameraController = class('core/EgoCameraController')
EgoCameraController:include(Controllable)
EgoCameraController:include(EventSource)


function EgoCameraController:initialize()
    self:initializeEventSource()
    self.orientation = Quat()
end

function EgoCameraController:destroy()
    self:destroyEventSource()
end

function EgoCameraController:getOrientation()
    return self.orientation
end

function EgoCameraController:_rotate( amount )
    local delta = Quat(amount)
    local orientation = self.orientation
    orientation = orientation * delta
    orientation = orientation:normalize()
    self.orientation = orientation
    self:fireEvent('orientation-updated', orientation)
end

EgoCameraController:mapControl('look-x', function( self, absolute, delta )
    self:_rotate(Vec(rad(delta), 0, 0))
end)

EgoCameraController:mapControl('look-y', function( self, absolute, delta )
    self:_rotate(Vec(0, rad(delta), 0))
end)


return EgoCameraController
