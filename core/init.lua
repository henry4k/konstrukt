-- Reimplement some functions

function print( ... )
    local args = {...}
    for i, arg in ipairs(args) do
        args[i] = tostring(arg)
    end
    ENGINE.Log(table.concat(args, '\t'))
end

function loadfile( fileName, ... )
    local data = ENGINE.ReadFile(fileName)
    return load(data, fileName, ...)
end

function dofile( fileName )
    return loadfile(fileName)()
end

require = dofile 'core/require.lua'
dofile 'core/table.lua'


-- Setup error handling

ENGINE.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Register exit key

local Control = require 'core/Control'
Control.registerKey('exit', function( pressed )
    if pressed then
        ENGINE.StopGameLoop()
    end
end)


-- Register resources

require 'core/Texture':registerResource()
require 'core/AudioBuffer':registerResource()
require 'core/Mesh':registerResource()
require 'core/Shader':registerResource()
require 'core/ShaderProgram':registerResource()

local ResourceManager = require 'core/ResourceManager'
ResourceManager.enableLoading(true)


-- Setup default render target

local Camera           = require 'core/Camera'
local ShaderProgram    = require 'core/ShaderProgram'
local ShaderProgramSet = require 'core/ShaderProgramSet'
local ModelWorld       = require 'core/ModelWorld'

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

ResourceManager.enableLoading(false)


-- Process command line arguments

local Scenario = require 'core/Scenario'

local interactive = false
local packages = {}

for i, argument in ipairs(ARGS) do
    if argument:match('-.*') then
        if argument == '-i' then
            interactive = true
        end
    else
        if argument:match('.+%.lua') then
            dofile(argument)
        else
            table.insert(packages, argument)
        end
    end
end

Scenario.load(packages[1], packages)

if interactive then
    debug.debug()
    ENGINE.StopGameLoop()
end
