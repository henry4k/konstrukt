--- @script core.bootstrap.init
--- Executed during engine initialization.


-- Setup error handling

ENGINE.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Initialize random generator

math.randomseed(ENGINE.GetTime())


-- Reimplement some functions
-- luacheck: globals print loadfile dofile

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
    return load(data, '@'..fileName, ...)
end

function _dofile( fileName )
    local chunk, errorMessage = _loadfile(fileName)
    if not chunk then
        error(errorMessage)
    end
    return chunk()
end

_dofile 'core/bootstrap/table.lua'
_dofile 'core/bootstrap/math.lua'
_dofile 'core/bootstrap/path.lua'
_dofile 'core/bootstrap/require.lua'


-- Register exit key

local Control        = require 'core/Control'
local GlobalControls = require 'core/GlobalControls'

Control.pushControllable(GlobalControls())

--- Stops the gameloop and thus stops the game.
-- @control exit
GlobalControls:mapControl('exit', function( self, absolute, delta )
    if delta > 0 then
        ENGINE.StopGameLoop()
    end
end)


-- Fire global event on engine shutdown
local GlobalEventSource = require 'core/GlobalEventSource'
ENGINE.SetEventCallback('Shutdown', function()
    GlobalEventSource:fireEvent('shutdown')
end)


-- Process command line arguments

local Scenario = require 'core/Scenario'

local interactive = false
local packages = {}

for _, argument in ipairs(ARGS) do
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
