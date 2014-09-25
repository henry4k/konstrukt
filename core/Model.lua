local class  = require 'middleclass'
local Vec    = require 'core/Vector'
local Mat4   = require 'core/Matrix4'
local DestroyModel              = ENGINE.DestroyModel
local SetModelAttachmentTarget  = ENGINE.SetModelAttachmentTarget
local SetModelMesh              = ENGINE.SetModelMesh
local SetModelTexture           = ENGINE.SetModelTexture
local SetModelProgramFamilyList = ENGINE.SetModelProgramFamilyList
local SetModelUniform           = ENGINE.SetModelUniform
local UnsetModelUniform         = ENGINE.UnsetModelUniform


--- Models are aggregations of meshes, textures, and shaders that can be used
-- to render something on the screen.
local Model = class('core/Model')

function Model:initialize( handle, renderLayerName )
    self.handle = handle
    self.renderLayerName = renderLayerName
    self.attachmentTarget = nil
end

function Model:destroy()
    DestroyModel(self.handle)
    self.handle = nil
end

function Model:getRenderLayerName()
    return self.renderLayerName
end

function Model:setAttachmentTarget( solid )
    SetModelAttachmentTarget(self.handle, solid.handle)
    self.attachmentTarget = solid
end

function Model:getAttachmentTarget()
    return self.attachmentTarget
end

function Model:setTransformation( transformation )
    SetModelTransformation(self.handle, transformation.handle)
end

function Model:setMesh( mesh )
    SetModelMesh(self.handle, mesh.handle)
end

function Model:setTexture( unit, texture )
    SetModelTexture(self.handle, unit, texture.handle)
end

function Model:setProgramFamilyList( familyList )
    SetModelProgramFamilyList(self.handle, familyList)
end

function Model:setUniform( name, value, type )
    if class.Object.isInstanceOf(value, Mat4) then
        SetModelUniform(self.handle, name, 'mat4', value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        SetModelUniform(self.handle, name, 'vec'..#value, value:unpack())
    else
        SetModelUniform(self.handle, name, type, value)
    end
end

function Model:unsetUniform( name )
    UnsetModelUniform(self.handle, name)
end


return Model
