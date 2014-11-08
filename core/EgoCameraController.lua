local class        = require 'middleclass'
local Vec          = require 'core/Vector'
local Quat         = require 'core/Quaternion'
local Controllable = require 'core/Controllable'
local EventSource  = require 'core/EventSource'


local nominalUp    = Vec(0, 1, 0)
local nominalRight = Vec(1, 0, 0)

local EgoCameraController = class('core/EgoCameraController')
EgoCameraController:include(Controllable)
EgoCameraController:include(EventSource)


function EgoCameraController:initialize()
    self:initializeEventSource()
    self.orientation = Quat()
    self.up = nominalUp:clone()
    self.right = nominalRight:clone()
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

    local up = Quat:multiplyVector(nominalUp, orientation)
    self.up = up:normalize()

    local right = Quat:multiplyVector(nominalRight, orientation)
    self.right = right:normalize()

    self:fireEvent('orientation-updated', orientation)
end

EgoCameraController:mapControl('look-x', function( self, absolute, delta )
    delta = delta * 0.01
    self:_rotate(self.up*delta)
end)

EgoCameraController:mapControl('look-y', function( self, absolute, delta )
    delta = delta * 0.01
    self:_rotate(self.right*delta)
end)


return EgoCameraController
