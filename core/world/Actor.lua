local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Control        = require 'core/Control'
local Controllable    = require 'core/Controllable'
local WorldObject    = require 'core/world/WorldObject'
local CameraManifold = require 'core/graphics/CameraManifold'
local EgoCameraController = require 'core/EgoCameraController'


local DefaultFoV = math.rad(80)
local ZoomedFoV = math.rad(10)


--- Things that the player can take control of.
local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controllable)

function Actor:initialize( renderTarget )
    WorldObject.initialize(self)

    Control.pushControllable(self)

    self.egoCameraController = EgoCameraController()
    Control.pushControllable(self.egoCameraController)
    local function onOrientationUpdated( self, orientation )
        self.cameraManifold:setViewTransformation(orientation:toMatrix())
    end
    self.egoCameraController:addEventTarget('orientation-updated', self, onOrientationUpdated)

    self.cameraManifold = CameraManifold(renderTarget)
    self.cameraManifold:setFieldOfView(DefaultFoV)
end

function Actor:destroy()
    Control.popControllable(self.egoCameraController)
    Control.popControllable(self)
    self.cameraManifold:destroy()
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
