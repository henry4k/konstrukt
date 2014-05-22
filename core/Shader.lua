local class = require 'middleclass'


local Shader = class('core/Shader')

function Shader:initialize( fileName )
    self.handle = NATIVE.LoadShader(fileName)
end


return Shader
