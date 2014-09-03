local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'


local Model = class('core/Model')

function Model:initialize( stage, shaderProgram )
    self.handle = NATIVE.CreateModel(stage, shaderProgram.handle)
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

function Model:setTexture( unit, texture )
    NATIVE.SetModelTexture(self.handle, unit, texture.handle)
end

function Model:setUniform( name, value, type )
    if value == nil then
        if class.Object.isInstanceOf(value, Mat4) then
            NATIVE.SetModelUniform(self.handle, name, 'mat4', value.handle)
        elseif class.Object.isInstanceOf(value, Vec) then
            NATIVE.SetModelUniform(self.handle, name, 'vec'..#value, value:unpack())
        else
            NATIVE.SetModelUniform(self.handle, name, type, value)
        end
    else
        NATIVE.UnsetModelUniform(self.handle, name)
    end
end


return Model
