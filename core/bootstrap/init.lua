--- @script core.bootstrap.init
--- Executed during engine initialization.

local modules, engine = ...


-- Setup error handling

engine.SetCallback.fn('error', function( message )
    return debug.traceback(message, 2)
end)

engine.SetCallback.fn('shutdown', function()
    print('Lua shutdown callback')
end)


-- Initialize random generator

math.randomseed(engine.GetTime.fn())


-- Reimplement some functions
-- luacheck: print loadfile dofile

coroutine = modules.taggedcoro.fortag'vanilla'

local Log = engine.Log.fn
function print( ... )
    local count = select('#', ...)
    local strings = {}
    for i = 1, count do
        strings[i] = tostring(select(i, ...))
    end
    Log('info', table.concat(strings, '\t'))
end

function loadfile( fileName, ... )
    error('loadfile is not available in Konstrukt, you must use Lua modules instead.')
end

function dofile( fileName )
    error('dofile is not available in Konstrukt, you must use Lua modules instead.')
end

local ReadFile = engine.ReadFile.fn

local function _loadfile( fileName, ... )
    local data = ReadFile(fileName)
    return load(data, '@'..fileName, ...)
end

local function _dofile( fileName, ... )
    local chunk = assert(_loadfile(fileName))
    return chunk(...)
end

_dofile 'core/bootstrap/table.lua'
_dofile 'core/bootstrap/math.lua'
_dofile 'core/bootstrap/path.lua'
_dofile('core/bootstrap/require.lua', modules, engine, _dofile)


local Scheduler = require 'core/Scheduler'
Scheduler._run(Scheduler.createScheduledCoroutine(function()
    -- Register exit key

    local engine         = require 'engine'
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
    -- TODO: This shall be done via an event
    if _scenario then
        Scenario.load(_scenario)
        _scenario = nil
    end
end))
