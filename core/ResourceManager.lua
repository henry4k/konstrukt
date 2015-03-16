--- @module core.ResourceManager
---

-- As the resource manager also loads lua scripts, this file may have no dependencies.


local ResourceManager = {
    loaders = {},
    resources = {},
    loadingEnabled = false
}

--- Registers a resource loader.
--
-- Resource loaders are functions or other callables.
-- @{core.ResourceManager.load} passes the parameters to the loader, which shall
-- return a table, nil if the requested resource doesn't exist or yield an error
-- if something went wrong.
--
-- The table contains:
--
-- - value: The loaded resource. (This is returned by the load and get functions.)
-- - destructor: Optional. Called when the resource shall be unloaded.
--
function ResourceManager.registerLoader( type, loader )
    if ResourceManager.loaders[type] then
        error('A loader for "'..type..'" has already been registered.')
    else
        ResourceManager.loaders[type] = loader
        print('Registered '..type..' loader.')
    end
end

--- Enables/disables resource loading. (Disabled by default.)
--
-- Attempting to load a resource, while loading is locked, will raise an error.
--
function ResourceManager.enableLoading( enabled )
    ResourceManager.loadingEnabled = enabled
end

--- Return a loaded resource.
--
-- @return
-- The resource or `nil` if the requested resource has not been loaded yet.
--
function ResourceManager.get( type, ... )
    local id = ResourceManager._createResourceIdentifier(type, ...)
    local resource = ResourceManager.resources[id]
    if resource then
        return resource.value
    else
        return nil
    end
end

local InaccessibleResourceMT = {
    __index = function( table, key )
        if key == 'value' then
            error(table.errorMessage)
        end
    end
}
local function CreateInaccessibleResource( errorFormat, ... )
    return setmetatable({ errorMessage = errorFormat:format(...) },
                        InaccessibleResourceMT)
end

--- Tries to load a resource if it hasn't been loaded yet.
--
-- @return
-- The resource or `nil` if the requested resource doesn't exist.
--
function ResourceManager.load( type, ... )
    local id = ResourceManager._createResourceIdentifier(type, ...)

    assert(ResourceManager.loadingEnabled,
           'Can\'t load resource "'..id..'", since resource loading is disabled.')

    local resourceValue = ResourceManager.get(type, ...)
    if not resourceValue then
        local loader = ResourceManager.loaders[type]
        if loader then
            ResourceManager.resources[id] = CreateInaccessibleResource(
                'Illegal access to %q as it\'t currently being loaded.', id)
            local resource = loader(...)
            ResourceManager.resources[id] = nil
            if resource then
                resourceValue = resource.value
                assert(resourceValue)
                ResourceManager.resources[id] = resource
                print('Loaded resource "'..id..'".')
            else
                print('Resource "'..id..'" doesn\'t exist.')
            end
        else
            error('No loader for resources of type "'..type..'".')
        end
    end
    return resourceValue
end

function ResourceManager._put( resource, type, ... )
    local id = ResourceManager._createResourceIdentifier(type, ...)
    ResourceManager.resources[id] = resource
end

--- Drops all loaded resources.
--
-- Also calls the resources destructor.
--
function ResourceManager.clear()
    for _, resource in pairs(ResourceManager.resources) do
        if resource.destructor then
            resource.destructor(resource.value)
        end
    end
    ResourceManager.resources = {}
end

--- Use the given parameters to create an unique identifier.
--
-- Each parameter set creates a unique identifier that equals only other
-- identifiers if they're created using the same set of parameters.
--
-- @internal
--
function ResourceManager._createResourceIdentifier( ... )
    local args = {...}
    local strings = {}
    for _,arg in ipairs(args) do
        if type(arg) == 'table' then
            local subargs = {}
            for k,v in pairs(arg) do
                table.insert(subargs, k)
                table.insert(subargs, v)
            end
            local str = ResourceManager._createResourceIdentifier(table.unpack(subargs))
            table.insert(strings, str)
        else
            table.insert(strings, tostring(arg))
        end
    end
    return table.concat(strings, ':')
end

return ResourceManager
