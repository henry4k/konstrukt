--- @classmod core.graphics.OrthographicCamera
--- An orthographic camera.
--
-- Extends @{core.graphics.Camera}.


local engine    = require 'engine'
local class     = require 'middleclass'
local Scheduler = require 'core/Scheduler'
local Camera    = require 'core/graphics/Camera'


local OrthographicCamera = class('core/graphics/OrthographicCamera', Camera)

function OrthographicCamera:initialize( modelWorld, lightWorld )
    Camera.initialize(self, modelWorld, lightWorld, 'orthographic')
end

--- Change the visible area.
function OrthographicCamera:setScale( scale )
    assert(scale > 0, 'Scale must be greater than zero.')
    Scheduler.blindCall(engine.SetCameraScale, self.handle, scale)
end


return OrthographicCamera
