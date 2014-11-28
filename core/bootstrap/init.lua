-- Setup error handling

ENGINE.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Initialize random generator

math.randomseed(ENGINE.GetTime())


-- Reimplement some functions

function print( ... )
    local args = {...}
    for i, arg in ipairs(args) do
        args[i] = tostring(arg)
    end
    ENGINE.Log(table.concat(args, '\t'))
end

function loadfile( fileName, ... )
    error('loadfile is not available in Apoapsis, you must use Lua modules instead.')
end

function dofile( fileName )
    error('dofile is not available in Apoapsis, you must use Lua modules instead.')
end

function _loadfile( fileName, ... )
    local data = ENGINE.ReadFile(fileName)
    return load(data, fileName, ...)
end

function _dofile( fileName )
    return _loadfile(fileName)()
end

_dofile 'core/bootstrap/table.lua'
_dofile 'core/bootstrap/logic.lua'
_dofile 'core/bootstrap/require.lua'


-- Register exit key

local class        = require 'middleclass'
local Control      = require 'core/Control'
local Controllable = require 'core/Controllable'

local GlobalControls = class('core/init/GlobalControls')
GlobalControls:include(Controllable)

function GlobalControls:exit( absolute, delta )
    if delta > 0 then
        ENGINE.StopGameLoop()
    end
end

GlobalControls:mapControl('exit', GlobalControls.exit)
Control.pushControllable(GlobalControls())


-- Setup chunk manager

local Shutdown       = require 'core/Shutdown'
local Vec            = require 'core/Vector'
local VoxelVolume    = require 'core/voxel/VoxelVolume'
local ChunkManager   = require 'core/world/ChunkManager'
local ChunkActivator = require 'core/world/ChunkActivator'

local voxelVolume = VoxelVolume(Vec(32, 32, 32))
chunkManager = ChunkManager(voxelVolume)

Shutdown.registerHandler(function()
    chunkManager:destroy()
    voxelVolume:destroy()
end)


-- Setup default render target

local ResourceManager   = require 'core/ResourceManager'
local Vec               = require 'core/Vector'
local Quat              = require 'core/Quaternion'
local Mat4              = require 'core/Matrix4'
local ModelWorld        = require 'core/graphics/ModelWorld'
local ShaderProgram     = require 'core/graphics/ShaderProgram'
local ShaderProgramSet  = require 'core/graphics/ShaderProgramSet'
local PerspectiveCamera = require 'core/graphics/PerspectiveCamera'
local Actor             = require 'core/world/Actor'

local foregroundModelWorld = ModelWorld()
local worldModelWorld      = ModelWorld()
local backgroundModelWorld = ModelWorld()

local foregroundCamera = PerspectiveCamera(foregroundModelWorld)
local worldCamera      = PerspectiveCamera(worldModelWorld)
local backgroundCamera = PerspectiveCamera(backgroundModelWorld)

ResourceManager.enableLoading(true)
local defaultShaderProgram = ShaderProgram:load('core/graphics/shaders/Default.vert',
                                                'core/graphics/shaders/Default.frag')
ResourceManager.enableLoading(false)
local defaultShaderProgramSet = ShaderProgramSet(defaultShaderProgram)

local defaultRT = require 'core/graphics/DefaultRenderTarget':get()
defaultRT:setCamera(0, 'foreground', foregroundCamera)
defaultRT:setCamera(2,      'world',      worldCamera)
defaultRT:setCamera(1, 'background', backgroundCamera)
defaultRT:setShaderProgramSet(defaultShaderProgramSet)

actor = Actor(defaultRT)


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
