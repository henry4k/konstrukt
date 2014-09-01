local class = require 'core/middleclass'
local ResourceManager = require 'core/ResourceManager'


--- Creates a shader by reading the given `fileName`.
-- The shader type is determined by the file extension automatically.
local Shader = class('core/Shader')

function Shader.static:registerResource()
    ResourceManager.registerLoader('core/Shader', function( fileName )
        return Shader:new(fileName)
    end)
end

function Shader:initialize( fileName )
    self.handle = NATIVE.LoadShader(fileName)
end

function Shader:destroy()
    NATIVE.DestroyShader(self.handle)
    self.handle = nil
end


return Shader
