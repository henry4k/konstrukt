--- Mixin that provides a more convenient middleclass integration of the resource loaders.
-- @module core.Resource


local ResourceManager = require 'core/ResourceManager'

local Resource = { static = {} }

function Resource:included( klass )
    ResourceManager.registerLoader(klass.name, function(...)
        return klass:_load(...)
    end)
end

---
-- @param ...
function Resource.static:load( ... )
    return ResourceManager.load(self.name, ...)
end

---
-- @param ...
function Resource.static:get( ... )
    return ResourceManager.get(self.name, ...)
end


return Resource
