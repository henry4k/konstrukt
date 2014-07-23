local class = require 'core/middleclass.lua'


local Shader = class('core/Shader')

function Shader:initialize( fileName )
    self.handle = NATIVE.LoadShader(fileName)
end


return Shader
