local class   = require 'middleclass'


local ChunkActivator = class('core/voxel/ChunkActivator')

function ChunkActivator:initialize( position, range )
    self.position = position
    self.range = range
end

function ChunkActivator:getPosition()
    return self.position
end

function ChunkActivator:getRange()
    return self.range
end


return ChunkActivator
