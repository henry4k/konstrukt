--- @script core.bootstrap.init
--- Executed during engine initialization.

-- luacheck: globals _engine taggedcoro
local engine = _engine


-- Setup error handling

engine.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Initialize random generator

math.randomseed(engine.GetTime())


-- Reimplement some functions
-- luacheck: globals coroutine print loadfile dofile

coroutine = taggedcoro.fortag'vanilla'

function print( ... )
    local count = select('#', ...)
    local strings = {}
    for i = 1, count do
        strings[i] = tostring(select(i, ...))
    end
    engine.Log('info', table.concat(strings, '\t'))
end

function loadfile( fileName, ... )
    error('loadfile is not available in Konstrukt, you must use Lua modules instead.')
end

function dofile( fileName )
    error('dofile is not available in Konstrukt, you must use Lua modules instead.')
end

local function _loadfile( fileName, ... )
    local data = engine.ReadFile(fileName)
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
_dofile = nil


local Scheduler = require 'core/Scheduler'
Scheduler._run(Scheduler.createScheduledCoroutine(function()
    -- Register exit key

    local Control        = require 'core/Control'
    local GlobalControls = require 'core/GlobalControls'

    Control.pushControllable(GlobalControls())

    --- Stops the gameloop and thus stops the game.
    -- @control exit
    GlobalControls:mapControl('exit', function( self, absolute, delta )
        if delta > 0 then
            engine.StopSimulation()
        end
    end)


    -- Fire global event on engine shutdown
    local GlobalEventSource = require 'core/GlobalEventSource'
    engine.SetEventCallback('Shutdown', function()
        GlobalEventSource:fireEvent('shutdown')
    end)


    -- Initialize scenario

    local Scenario = require 'core/Scenario'

    -- luacheck: globals _scenario
    if _scenario then
        Scenario.load(_scenario)
        _scenario = nil
    end
end))
