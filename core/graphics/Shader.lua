--- Creates a shader by reading the given `fileName`.
-- The shader type is determined by the file extension automatically.
--
-- @module core.graphics.Shader


local class    = require 'middleclass'
local Resource = require 'core/Resource'
local LoadShader    = ENGINE.LoadShader
local DestroyShader = ENGINE.DestroyShader


local Shader = class('core/graphics/Shader')
Shader:include(Resource)

---
-- @local
function Shader.static:_load( fileName )
    local shader = Shader(fileName)
    return { value=shader, destructor=shader.destroy }
end

---
-- @param fileName
function Shader:initialize( fileName )
    self.handle = LoadShader(fileName)
end

---
function Shader:destroy()
    DestroyShader(self.handle)
    self.handle = nil
end


return Shader
