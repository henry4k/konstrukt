--- @script core.bootstrap.require
--- Implements @{require} because Konstrukt doesn't access the file system directly.

local modules, engine, _dofile = ...

-- XXX: Temporary dummy function:
engine.SetEventCallback = {
    fn = function() end
}

local ResourceManager = _dofile 'core/ResourceManager.lua'

local searchPaths = {
    '%s.lua',
    'core/third-party/%s.lua'
}

local GetFileType = engine.GetFileType.fn

ResourceManager.registerLoader('module', function( moduleName )
    for _, searchPath in ipairs(searchPaths) do
        local path = string.format(searchPath, moduleName)
        if GetFileType(path) == 'regular' then
            local module = _dofile(path)
            assert(module, 'Module at '..path..' did not return anything.')
            return { value=module }
        end
    end
    return nil
end)

ResourceManager._put({ value=ResourceManager }, 'module', 'core/ResourceManager')
for moduleName, module in pairs(modules) do
    ResourceManager._put({ value=module }, 'module', moduleName)
end

local rawengine = {}
for name, desc in pairs(engine) do
    rawengine[name] = desc.fn
end
ResourceManager._put({ value=rawengine }, 'module', 'rawengine')

function require( moduleName )
    local module = ResourceManager.get('module', moduleName) or
                   ResourceManager.load('module', moduleName)
    if not module then
        error(string.format("module '%s' not found", moduleName))
    end
    return module
end

local function wrapEngine()
    local Scheduler = require 'core/Scheduler'
    local awaitCall = Scheduler.awaitCall
    local blindCall = Scheduler.blindCall

    local wrappedEngine = {}
    for name, desc in pairs(engine) do
        local originalFn = desc.fn
        local wrappedFn

        if desc.thread_safe then
            wrappedFn = originalFn
        else
            if no_results then
                wrappedFn = function(...)
                    return blindCall(originalFn, ...)
                end
            else
                wrappedFn = function(...)
                    return awaitCall(originalFn, ...)
                end
            end
        end

        wrappedEngine[name] = wrappedFn
    end

    return wrappedEngine
end

-- Load all core modules:
ResourceManager.allowLoading(true)
ResourceManager._put({ value=wrapEngine() }, 'module', 'engine')
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
