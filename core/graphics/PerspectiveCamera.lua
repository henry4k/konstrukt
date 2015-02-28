--- @classmod core.graphics.PerspectiveCamera
--- A perspectivic camera.
--
-- Extends @{Camera}.

local assert = assert
local class  = require 'middleclass'
local Camera  = require 'core/graphics/Camera'
local SetCameraFieldOfView = ENGINE.SetCameraFieldOfView


local PerspectiveCamera = class('core/graphics/PerspectiveCamera', Camera)

function PerspectiveCamera:initialize( modelWorld )
    Camera.initialize(self, modelWorld, 'perspective')
end

--- Changes the cameras field of view.
function PerspectiveCamera:setFieldOfView( fov )
    assert(fov > 0, 'FoV must be greater than zero.')
    SetCameraFieldOfView(self.handle, fov)
end


return PerspectiveCamera
