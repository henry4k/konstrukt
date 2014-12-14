--- @classmod core.graphics.PerspectiveCamera

local assert = assert
local class  = require 'middleclass'
local Camera  = require 'core/graphics/Camera'
local SetCameraFieldOfView = ENGINE.SetCameraFieldOfView


local PerspectiveCamera = class('core/graphics/PerspectiveCamera', Camera)

---
-- @param modelWorld
function PerspectiveCamera:initialize( modelWorld )
    Camera.initialize(self, modelWorld, 'perspective')
end


---
-- @param fov
function PerspectiveCamera:setFieldOfView( fov )
    assert(fov > 0, 'FoV must be greater than zero.')
    SetCameraFieldOfView(self.handle, fov)
end


return PerspectiveCamera
