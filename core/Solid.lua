local class = require 'middleclass'


local Solid = class('core/Solid')

function Solid:initialize()
    self.handle = NATIVE.CreateSolid()
end

function Solid:remove()
    NATIVE.RemoveSolid(self.handle)
    self.handle = nil
end


return Solid
