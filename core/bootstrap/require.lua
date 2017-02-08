--- @script core.bootstrap.require
--- Implements @{require} because Konstrukt doesn't access the file system directly.

-- luacheck: globals require cjson _engine

local engine = _engine
local ResourceManager = _dofile 'core/ResourceManager.lua'

local searchPaths = {
    '%s.lua',
    'core/third-party/%s.lua',
}

ResourceManager.registerLoader('module', function( moduleName )
    for _, searchPath in ipairs(searchPaths) do
        local path = string.format(searchPath, moduleName)
        if engine.GetFileInfo(path) then
            local module = _dofile(path)
            assert(module, 'Module at '..path..' did not return anything.')
            return { value=module }
        end
    end
    return nil
end)

ResourceManager._put({ value=ResourceManager }, 'module', 'core/ResourceManager')
ResourceManager._put({ value=cjson },           'module', 'cjson')
ResourceManager._put({ value=engine },          'module', 'engine')
cjson  = nil
_engine = nil

function require( moduleName )
    local module = ResourceManager.get('module', moduleName) or
                   ResourceManager.load('module', moduleName)
    if not module then
        error(string.format("module '%s' not found", moduleName))
    end
    return module
end

-- Load all core modules:
ResourceManager.allowLoading(true)
local FS = require 'core/FileSystem'
for path in FS.matchingFiles('core/.*%.lua') do
    if not path:match('core/bootstrap/.*') then
        local moduleName = path:match('(.*)%.lua')
        require(moduleName)
    end
end
ResourceManager.allowLoading(false)

local GlobalEventSource = require 'core/GlobalEventSource'
GlobalEventSource:addEventTarget('shutdown', ResourceManager, ResourceManager.clear)
