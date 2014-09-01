local Shutdown = require 'core/Shutdown'


local ResourceManager = {
    loaders = {},
    resources = {}
}

--- Registers a resource loader.
-- Resource loaders are functions or other callables.
-- #ResourceManager.load passes the parameters to the loader, which shall return
-- the loaded resource, nil if the requested resource doesn't exist or yield an
-- error if something went wrong.
function ResourceManager.registerLoader( type, loader )
    if ResourceManager.loaders[type] then
        error('A loader for "'..type..'" has already been registered.')
    else
        ResourceManager.loaders[type] = loader
        print('Registered '..type..' loader.')
    end
end

--- Return a loaded resource.
-- @return
-- The resource or nil if the requested resource has not been loaded yet.
function ResourceManager.get( type, ... )
    local id = ResourceManager.createResourceIdentifier_(type, ...)
    return ResourceManager.resources[id]
end

--- Tries to load a resource if it hasn't been loaded yet.
-- @return
-- The resource or nil if the requested resource doesn't exist.
function ResourceManager.load( type, ... )
    local resource = ResourceManager.get(type, ...)
    if not resource then
        local loader = ResourceManager.loaders[type]
        if loader then
            resource = loader(...)
            local id = ResourceManager.createResourceIdentifier_(type, ...)
            if resource then
                ResourceManager.resources[id] = resource
                print('Loaded resource "'..id..'".')
            else
                print('Resource "'..id..'" doesn\'t exist.')
            end
        else
            error('No loader for resources of type "'..type..'".')
        end
    end
    return resource
end

--- Drops all loaded resources.
-- Tries to call #destroy in all resource objects.
function ResourceManager.clear()
    for _,resource in pairs(ResourceManager.resources) do
        if resource['destroy'] then
            resource:destroy()
        end
    end
    ResourceManager.resources = {}
end
Shutdown.registerHandler(ResourceManager.clear)

--- Use the given parameters to create an unique identifier.
-- Each parameter set creates a unique identifier that equals only other
-- identifiers if they're created using the same set of parameters.
function ResourceManager.createResourceIdentifier_( ... )
    local args = {...}
    local strings = {}
    for _,arg in ipairs(args) do
        if type(arg) == 'table' then
            local subargs = {}
            for k,v in pairs(arg) do
                table.insert(subargs, k)
                table.insert(subargs, v)
            end
            local str = ResourceManager.createResourceIdentifier_(table.unpack(subargs))
            table.insert(strings, str)
        else
            table.insert(strings, tostring(arg))
        end
    end
    return table.concat(strings, ':')
end

return ResourceManager