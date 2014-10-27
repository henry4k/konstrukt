local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Control        = require 'core/Control'
local Controlable    = require 'core/Controlable'
local WorldObject    = require 'core/world/WorldObject'
local CameraManifold = require 'core/graphics/CameraManifold'
local EgoCameraController = require 'core/EgoCameraController'


--- Things that the player can take control of.
local Actor = class('core/world/Actor', WorldObject)
Actor:include(Controlable)

function Actor:initialize( renderTarget )
    WorldObject.initialize(self)

    Control.pushControlable(self)

    self.egoCameraController = EgoCameraController()
    Control.pushControlable(self.egoCameraController)
    local function onOrientationUpdated( self, orientation )
        print(tostring(self)..': Updated orientation.')
        self.cameraManifold:setViewTransformation(orientation:toMatrix())
    end
    self.egoCameraController:addEventTarget('orientation-updated', self, onOrientationUpdated)

    self.cameraManifold = CameraManifold(renderTarget)
    self.cameraManifold:setFieldOfView(80)
end

function Actor:destroy()
    Control.popControlable(self.egoCameraController)
    Control.popControlable(self)
    self.cameraManifold:destroy()
    WorldObject.destroy(self)
end

Actor:mapControl('activate', function( self, absolute, delta )
    print(tostring(delta))
    if delta > 0 then
        print(tostring(self)..' activates something!')
    end
end)


return Actor
