local class = require 'middleclass'


local Model = class('core/Model')

function Model:initialize( shaderProgram )
    self.handle = NATIVE.CreateModel(shaderProgram.handle)
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
