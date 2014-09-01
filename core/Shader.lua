local class = require 'core/middleclass'
local Resource = require 'core/Resource'


--- Creates a shader by reading the given `fileName`.
-- The shader type is determined by the file extension automatically.
local Shader = class('core/Shader')
Shader:include(Resource)

function Shader.static:load( fileName )
    return Shader(fileName)
end

function Shader:initialize( fileName )
    self.handle = NATIVE.LoadShader(fileName)
end

function Shader:destroy()
    NATIVE.DestroyShader(self.handle)
    self.handle = nil
end


return Shader
