local class = require 'middleclass'


local Model = class('core/Model')

function Model:initialize( shaderProgram )
    self.handle = NATIVE.CreateModel(shaderProgram.handle)
end

function Model:remove()
    error('Model:remove() is unimplemented!')
    self.handle = nil
end

function Model:setTransformation( transformation )
    NATIVE.SetModelTransformation(self.handle, transformation.handle)
end

function Model:setMesh( mesh )
    NATIVE.SetModelMesh(self.handle, mesh.handle)
end

function Model:setTexture( texture )
    NATIVE.SetModelTexture(self.handle, texture.handle)
end

function Model:setTexture( texture )
    NATIVE.SetModelTexture(self.handle, texture.handle)
end

function Model:setUniform( name, ... )
    NATIVE.SetModelUniform(self.handle, name, ...)
end

function Model:unsetUniform( name )
    NATIVE.UnsetModelUniform(self.handle, name)
end


return Model
