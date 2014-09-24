local engine = require 'apoapsis.engine'

-- Setup error handling

engine.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Register exit key

local Control = require 'apoapsis.core.Control'
Control.registerKey('exit', function( pressed )
    if pressed then
        engine.Shutdown()
    end
end)


-- Register resources

require 'apoapsis.core.Texture':registerResource()
require 'apoapsis.core.AudioBuffer':registerResource()
require 'apoapsis.core.Mesh':registerResource()
require 'apoapsis.core.Shader':registerResource()
require 'apoapsis.core.ShaderProgram':registerResource()


-- Setup default render target

local Camera           = require 'apoapsis.core.Camera'
local ShaderProgram    = require 'apoapsis.core.ShaderProgram'
local ShaderProgramSet = require 'apoapsis.core.ShaderProgramSet'
local ModelWorld       = require 'apoapsis.core.ModelWorld'

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

local defaultShaderProgram = ShaderProgram:load('apoapsis/core/Shaders/Test.vert',
                                                'apoapsis/core/Shaders/Test.frag')
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'apoapsis.core.DefaultRenderTarget':get()
defaultRT:setCamera(defaultCamera)
defaultRT:setShaderProgramSet(defaultShaderProgramSet)
