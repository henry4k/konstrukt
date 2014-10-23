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

dofile 'core/require.lua'
dofile 'core/table.lua'
dofile 'core/logic.lua'


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

local PerspectiveCamera = require 'core/PerspectiveCamera'
local ShaderProgram    = require 'core/ShaderProgram'
local ShaderProgramSet = require 'core/ShaderProgramSet'
local ModelWorld       = require 'core/ModelWorld'
local Vec              = require 'core/Vector'
local Quat             = require 'core/Quaternion'
local Mat4             = require 'core/Matrix4'

local foregroundModelWorld = ModelWorld()
local worldModelWorld      = ModelWorld()
local backgroundModelWorld = ModelWorld()

local foregroundCamera = PerspectiveCamera(foregroundModelWorld)
local worldCamera      = PerspectiveCamera(worldModelWorld)
local backgroundCamera = PerspectiveCamera(backgroundModelWorld)

foregroundCamera:setFieldOfView(math.rad(80))
worldCamera:setFieldOfView(math.rad(80))
backgroundCamera:setFieldOfView(math.rad(80))

local function UpdateCameras( lookX, lookY )
    local m = Mat4()

    worldCamera:setViewTransformation(m)
end

LookX = 0
LookY = 0

Control.registerAxis('look-x', function( absolute, delta )
    LookX = absolute
    UpdateCameras(LookX, LookY)
end)

Control.registerAxis('look-y', function( absolute, delta )
    LookY = absolute
    UpdateCameras(LookX, LookY)
end)

local defaultShaderProgram = ShaderProgram:load('core/shaders/Default.vert',
                                                'core/shaders/Default.frag')
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'core/DefaultRenderTarget':get()
defaultRT:setCamera(0, 'foreground', foregroundCamera)
defaultRT:setCamera(1,      'world',      worldCamera)
defaultRT:setCamera(2, 'background', backgroundCamera)
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

if #packages > 0 then
    Scenario.load(packages[1], packages)
end

if interactive then
    debug.debug()
    ENGINE.StopGameLoop()
end
