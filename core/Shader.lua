local class = require 'core/middleclass'


--- Creates a shader by reading the given `fileName`.
-- The shader type is determined by the file extension automatically.
local Shader = class('core/Shader')

function Shader:initialize( fileName )
    self.handle = NATIVE.LoadShader(fileName)
end

function Shader:destroy()
    NATIVE.DestroyShader(self.handle)
    self.handle = nil
end


return Shader
