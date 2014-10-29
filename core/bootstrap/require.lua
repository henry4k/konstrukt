local ResourceManager = _dofile 'core/ResourceManager.lua'

local searchPaths = {
    '%s.lua',
    '%s/init.lua',
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

ResourceManager._put(ResourceManager, 'module', 'core/ResourceManager')

function require( moduleName )
    return ResourceManager.load('module', moduleName)
end

local Shutdown = require 'core/Shutdown'
Shutdown.registerHandler(ResourceManager.clear)
