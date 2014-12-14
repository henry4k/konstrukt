--- Things that the player can take control of.
-- @module core.world.Actor


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local Control             = require 'core/Control'
local Controllable        = require 'core/Controllable'
local WorldObject         = require 'core/world/WorldObject'
local CameraManifold      = require 'core/graphics/CameraManifold'
local EgoCameraController = require 'core/EgoCameraController'


local DefaultFoV = math.rad(80)
local ZoomedFoV  = math.rad(10)


local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controllable)

function Actor:initialize( renderTarget )
    WorldObject.initialize(self)
    self:initializeControllable()

    Control.pushControllable(self)

    self.cameraManifold = CameraManifold(renderTarget)
    self.cameraManifold:setFieldOfView(DefaultFoV)

    self.egoCameraController = EgoCameraController()
    Control.pushControllable(self.egoCameraController)
    local function onOrientationUpdated( self, orientation )
        local transformation = orientation:toMatrix():scale(Vec(1,1,-1))
        -- Invert Z axis to remain in right-handed system.
        self.cameraManifold:setViewTransformation(transformation)
    end
    self.egoCameraController:addEventTarget('orientation-updated', self, onOrientationUpdated)

    onOrientationUpdated(self, self.egoCameraController:getOrientation())
end

function Actor:destroy()
    Control.popControllable(self.egoCameraController)
    Control.popControllable(self)
    self.cameraManifold:destroy()
    self:destroyControllable()
    WorldObject.destroy(self)
end

Actor:mapControl('zoom', function( self, absolute, delta )
    if delta > 0 then
        self.cameraManifold:setFieldOfView(ZoomedFoV)
    else
        self.cameraManifold:setFieldOfView(DefaultFoV)
    end
end)


return Actor
