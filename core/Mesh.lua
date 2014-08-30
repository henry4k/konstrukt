local class = require 'core/middleclass'


local Mesh = class('core/Mesh')

function Mesh:initialize( meshBuffer )
    self.handle = NATIVE.CreateMesh(meshBuffer.handle)
end

function Mesh:destroy()
    NATIVE.DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
