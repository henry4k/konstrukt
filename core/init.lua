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


-- Setup default render target

local Camera           = require 'core/Camera'
local ShaderProgram    = require 'core/ShaderProgram'
local ShaderProgramSet = require 'core/ShaderProgramSet'
local ModelWorld       = require 'core/ModelWorld'

local astronomicalUnit = 149597870700
local au = astronomicalUnit

local defaultModelWorld = ModelWorld()
local defaultCamera = Camera(defaultModelWorld)
defaultCamera:setFieldOfView(math.rad(80))

local defaultShaderProgram = ShaderProgram:load('core/Shaders/Test.vert',
                                                'core/Shaders/Test.frag')
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'core/DefaultRenderTarget':get()
defaultRT:setCamera(defaultCamera)
defaultRT:setShaderProgramSet(defaultShaderProgramSet)


-- Process command line arguments

local function LoadPackage( module )
    ENGINE.MountPackage(module)
end

local interactive = false

for i, argument in ipairs(ARGS) do
    if argument:match('-.*') then
        if argument == '-i' then
            interactive = true
        end
    else
        if argument:match('.+%.lua') then
            dofile(argument)
        else
            LoadPackage(argument)
        end
    end
end

if interactive then
    debug.debug()
    ENGINE.StopGameLoop()
end
