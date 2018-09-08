--- @script core.bootstrap.init
--- Executed during engine initialization.

-- luacheck: globals _engine
local engine = _engine


-- Setup error handling

engine.SetErrorFunction(function( message )
    return debug.traceback(message, 2)
end)


-- Reimplement some functions
-- luacheck: globals print loadfile dofile

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

_dofile'core/bootstrap/table.lua'
_dofile'core/bootstrap/math.lua'
_dofile'core/bootstrap/path.lua'
_dofile'core/bootstrap/require.lua'

_dofile'core/bootstrap/test.lua'

_dofile = nil
