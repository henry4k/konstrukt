--- @classmod core.graphics.PerspectiveCamera
--- A perspectivic camera.
--
-- Extends @{core.graphics.Camera}.

local engine    = require 'engine'
local class     = require 'middleclass'
local Camera    = require 'core/graphics/Camera'
local Scheduler = require 'core/Scheduler'


local PerspectiveCamera = class('core/graphics/PerspectiveCamera', Camera)

function PerspectiveCamera:initialize( modelWorld, lightWorld )
    Camera.initialize(self, modelWorld, lightWorld, 'perspective')
end

--- Changes the cameras field of view.
function PerspectiveCamera:setFieldOfView( fov )
    assert(fov > 0, 'FoV must be greater than zero.')
    Scheduler.blindCall(engine.SetCameraFieldOfView, self.handle, fov)
end


return PerspectiveCamera
