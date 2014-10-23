local assert = assert
local class  = require 'middleclass'
local Camera  = require 'core/Camera'
local SetCameraFieldOfView = ENGINE.SetCameraFieldOfView


local PerspectiveCamera = class('core/PerspectiveCamera')

function PerspectiveCamera:initialize( modelWorld )
    Camera.initialize(self, modelWorld, 'perspective')
end

function PerspectiveCamera:setFieldOfView( fov )
    assert(fov > 0, 'FoV must be greater than zero.')
    SetCameraFieldOfView(self.handle, fov)
end


return PerspectiveCamera
