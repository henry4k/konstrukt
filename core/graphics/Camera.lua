--- @classmod core.graphics.Camera
--- Cameras define the transformation which is used to render the associated @{ModelWorld} to the @{RenderTarget}.
--
-- Includes @{core.physics.HasAttachmentTarget}.


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Scheduler           = require 'core/Scheduler'
local Mat4                = require 'core/Matrix4'
local ModelWorld          = require 'core/graphics/ModelWorld'
local LightWorld          = require 'core/graphics/LightWorld'
local ShaderVariableSet   = require 'core/graphics/ShaderVariableSet'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'


local Camera = class('core/graphics/Camera')
Camera:include(HasAttachmentTarget)

--- Abstract class.
-- Use @{core.graphics.PerspectiveCamera} or @{core.graphics.OrthographicCamera} instead.
function Camera:initialize( modelWorld, lightWorld, projectionType )
    assert(self.class ~= Camera, 'Camera is an abstract class and not meant to be instanciated directly.')
    assert(Object.isInstanceOf(modelWorld, ModelWorld), 'Must be initialized with a model world.')
    local lightWorldHandle
    if lightWorld then
        assert(Object.isInstanceOf(lightWorld, LightWorld), 'Must be initialized with a light world.')
        lightWorldHandle = lightWorld.handle
    end
    Scheduler.awaitCall(function()
        self.handle = engine.CreateCamera(modelWorld.handle, lightWorldHandle)
        self.modelWorld = modelWorld
        self.lightWorld = lightWorld
        self.shaderVariables =
            ShaderVariableSet(engine.GetCameraShaderVariableSet(self.handle))
        engine.SetCameraProjectionType(self.handle, projectionType)
    end)
end

function Camera:destroy()
    Scheduler.blindCall(engine.DestroyCamera, self.handle)
    self.handle = nil
    self.shaderVariables:destroy()
    self.shaderVariables = nil
end

--- Retrieve @{core.graphics.ModelWorld} used by the camera.
function Camera:getModelWorld()
    return self.modelWorld
end

--- Retrieve @{core.graphics.LightWorld} used by the camera.
function Camera:getLightWorld()
    return self.lightWorld
end

--- Change the cameras view transformation matrix.
function Camera:setViewTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    Scheduler.blindCall(engine.SetCameraViewTransformation, self.handle, matrix.handle)
end

--- Set the cameras view range.
--
-- Due to the design of the rendering system, cameras can't display infinitly
-- near or far objects.
--
function Camera:setNearAndFarPlanes( zNear, zFar )
    assert(zNear > 0 and zFar > 0, 'z near and z far must be greater than zero.')
    assert(zFar > zNear, 'z far must be larger than z near.')
    Scheduler.blindCall(engine.SetCameraNearAndFarPlanes, self.handle, zNear, zFar)
end

function Camera:_setAttachmentTarget( solidHandle, flags )
    Scheduler.blindCall(engine.SetCameraAttachmentTarget, self.handle, solidHandle, flags)
end


return Camera
