local assert = assert
local class  = require 'middleclass'
local Camera  = require 'core/graphics/Camera'
local SetCameraScale = ENGINE.SetCameraScale


local OrthographicCamera = class('core/graphics/OrthographicCamera', Camera)

function OrthographicCamera:initialize( modelWorld )
    Camera.initialize(self, modelWorld, 'orthographic')
end

function OrthographicCamera:setScale( scale )
    assert(scale > 0, 'Scale must be greater than zero.')
    SetCameraScale(self.handle, scale)
end


return OrthographicCamera
