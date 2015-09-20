local class   = require 'middleclass'


local SolidChunkActivator = class('core/voxel/SolidChunkActivator')

function SolidChunkActivator:initialize( solid, range )
    self.solid = solid
    self.range = range
end

function SolidChunkActivator:getPosition()
    return self.solid:getPosition()
end

function SolidChunkActivator:getRange()
    return self.range
end


return SolidChunkActivator
