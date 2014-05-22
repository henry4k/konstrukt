local class = require 'middleclass'


local Model = class('core/Model')

function Model:initialize( fileName )
    self.handle = NATIVE.CreateModel()
end

function Model:setMesh( mesh )
    NATIVE.SetModelMesh(self.handle, mesh.handle)
end

function Model:setTransformation( transformation )
    NATIVE.SetModelTransformation(self.handle, transformation.handle)
end

function Model:remove()
    NATIVE.RemoveModel(self.handle)
    self.handle = nil
end


return Model
