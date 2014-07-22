local class = require 'middleclass'


local Solid = class('core/Solid')

function Solid:initialize()
    self.handle = NATIVE.CreateSolid()
end

function Solid:getPosition()
    NATIVE.GetSolidPosition(self.handle)
end

function Solid:getRotation()
    NATIVE.GetSolidRotation(self.handle)
end


return Solid
