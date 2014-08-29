local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'


local Model = class('core/Model')

function Model:initialize( shaderProgram )
    self.handle = NATIVE.CreateModel(shaderProgram.handle)
end

function Model:destroy()
    NATIVE.DestroyModel(self.handle)
    self.handle = nil
end

function Model:setAttachmentTarget( target )
    NATIVE.SetModelAttachmentTarget(self.handle, target.handle)
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

function Model:setUniform( name, value )
    if class.Object.isInstanceOf(value, Mat4) then
        NATIVE.SetModelUniform(self.handle, name, value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        NATIVE.SetModelUniform(self.handle, name, value:unpack())
    else
        NATIVE.SetModelUniform(self.handle, name, value)
    end
end

function Model:unsetUniform( name )
    NATIVE.UnsetModelUniform(self.handle, name)
end


return Model
