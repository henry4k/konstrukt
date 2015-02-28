--- @mixin core.Resource
--- Provides a more convenient middleclass integration of the resource loaders.
--
-- Classes that include the mixin must provide a static `_load` function, which
-- accepts some parameters describing the resource.
-- See @{ResourceManager.registerLoader}.


local ResourceManager = require 'core/ResourceManager'

local Resource = { static = {} }

function Resource:included( klass )
    ResourceManager.registerLoader(klass.name, function(...)
        return klass:_load(...)
    end)
end

--- Shortcut for loading a resource.
-- @see core.ResourceManager.load
-- @usage skyTexture = Texture:load('sky.png')
function Resource.static:load( ... )
    return ResourceManager.load(self.name, ...)
end

--- Shortcut for getting a resource.
-- @see core.ResourceManager.get
-- @usage skyTexture = Texture:get('sky.png')
function Resource.static:get( ... )
    return ResourceManager.get(self.name, ...)
end


return Resource
