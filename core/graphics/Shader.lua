local class    = require 'middleclass'
local Resource = require 'core/Resource'
local LoadShader    = ENGINE.LoadShader
local DestroyShader = ENGINE.DestroyShader


--- Creates a shader by reading the given `fileName`.
-- The shader type is determined by the file extension automatically.
local Shader = class('core/graphics/Shader')
Shader:include(Resource)

function Shader.static:load( fileName )
    return Shader(fileName)
end

function Shader:initialize( fileName )
    self.handle = LoadShader(fileName)
end

function Shader:destroy()
    DestroyShader(self.handle)
    self.handle = nil
end


return Shader
