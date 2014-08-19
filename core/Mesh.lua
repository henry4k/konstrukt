local class = require 'core/middleclass'


local Mesh = class('core/Mesh')

function Mesh:initialize( meshBuffer )
    self.handle = NATIVE.CreateMesh(meshBuffer.handle)
end


return Mesh
