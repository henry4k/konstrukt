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

local defaultModelWorld = ModelWorld()
local defaultCamera = Camera(defaultModelWorld)
defaultCamera:setFieldOfView(math.rad(80))

local defaultShaderProgram = ShaderProgram:load('core/Shaders/Test.vert',
                                                'core/Shaders/Test.frag')
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'core/DefaultRenderTarget':get()
defaultRT:setCamera(defaultCamera)
defaultRT:setShaderProgramSet(defaultShaderProgramSet)
