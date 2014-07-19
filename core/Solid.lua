local class = require 'middleclass'


local Solid = class('core/Solid')

function Solid:initialize()
    self.handle = NATIVE.CreateSolid()
end


return Solid
