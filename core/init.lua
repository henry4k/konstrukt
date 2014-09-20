-- Setup error handling

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)


-- Register resources:

require 'core/Texture':registerResource()
require 'core/AudioBuffer':registerResource()
require 'core/Mesh':registerResource()
require 'core/Shader':registerResource()
require 'core/ShaderProgram':registerResource()


-- Setup default render target

local Camera        = require 'core/Camera'
local ShaderProgram = require 'core/ShaderProgram'
local ShaderProgramSet = require 'core/ShaderProgramSet'
local ModelWorld    = require 'core/ModelWorld'

local astronomicalUnit = 149597870700
local au = astronomicalUnit

local defaultRenderLayers = {
    { name='background', zNear=1000,  zFar=20*au },
    { name='default',    zNear=0.1,   zFar=100.0 },
    { name='foreground', zNear=0.001, zFar=1.0   }
}
local defaultModelWorld = ModelWorld(defaultRenderLayers)
local defaultCamera = Camera(defaultModelWorld)
defaultCamera:setFieldOfView(math.rad(80))

local defaultShaderProgram = ShaderProgram:load('core/Shaders/Test.vert',
                                                'core/Shaders/Test.frag')
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'core/DefaultRenderTarget':get()
defaultRT:setCamera(defaultCamera)
defaultRT:setShaderProgramSet(defaultShaderProgramSet)
