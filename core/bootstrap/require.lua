--- @script core.bootstrap.require
--- Implements @{require} because Apoapsis doesn't access the file system directly.

-- luacheck: globals require

local ResourceManager = _dofile 'core/ResourceManager.lua'

local searchPaths = {
    '%s.lua',
    'core/third-party/%s.lua',
}

ResourceManager.registerLoader('module', function( moduleName )
    for _, searchPath in ipairs(searchPaths) do
        local path = string.format(searchPath, moduleName)
        if ENGINE.FileExists(path) then
            local module = _dofile(path)
            assert(module, 'Module at '..path..' did not return anything.')
            return { value=module }
        end
    end
    return nil
end)

ResourceManager._put({ value=ResourceManager }, 'module', 'core/ResourceManager')

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

local Shutdown = require 'core/Shutdown'
Shutdown.registerHandler(ResourceManager.clear)
