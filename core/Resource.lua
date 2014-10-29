local ResourceManager = require 'core/ResourceManager'


--- Provides a more convenient middleclass integration of the resource loaders.
local Resource = { static = {} }

function Resource:included( klass )
    ResourceManager.registerLoader(klass.name, function(...)
        return klass:_load(...)
    end)
end

function Resource.static:load( ... )
    return ResourceManager.load(self.name, ...)
end

function Resource.static:get( ... )
    return ResourceManager.get(self.name, ...)
end


return Resource
