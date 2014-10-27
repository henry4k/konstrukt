local assert  = assert
local isInteger = math.isInteger
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'
local Mat4    = require 'core/Matrix4'
local Solid   = require 'core/physics/Solid'
local Mesh    = require 'core/graphics/Mesh'
local Texture = require 'core/graphics/Texture'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'
local DestroyModel              = ENGINE.DestroyModel
local SetModelAttachmentTarget  = ENGINE.SetModelAttachmentTarget
local SetModelTransformation    = ENGINE.SetModelTransformation
local SetModelMesh              = ENGINE.SetModelMesh
local SetModelTexture           = ENGINE.SetModelTexture
local SetModelProgramFamilyList = ENGINE.SetModelProgramFamilyList
local SetModelUniform           = ENGINE.SetModelUniform
local UnsetModelUniform         = ENGINE.UnsetModelUniform


--- Models are aggregations of meshes, textures, and shaders that can be used
-- to render something on the screen.
local Model = class('core/graphics/Model')
Model:include(HasTransformation)
Model:include(HasAttachmentTarget)

-- DON'T CALL THIS DIRECTLY!  Use ModelWorld:createModel() instead.
function Model:initialize( handle )
    self.handle = handle
    self.attachmentTarget = nil
end

function Model:destroy()
    DestroyModel(self.handle)
    self.handle = nil
end

function Model:setMesh( mesh )
    assert(Object.isInstanceOf(mesh, Mesh), 'Must be called with a mesh.')
    SetModelMesh(self.handle, mesh.handle)
end

function Model:setTexture( unit, texture )
    assert(isInteger(unit), 'Unit must be an integer.')
    assert(unit >= 0, 'Unit must be positive.')
    assert(Object.isInstanceOf(texture, Texture), 'Must be called with a texture.')
    SetModelTexture(self.handle, unit, texture.handle)
end

function Model:setProgramFamilyList( familyList )
    SetModelProgramFamilyList(self.handle, familyList)
end

function Model:setUniform( name, value, type )
    if Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
        SetModelUniform(self.handle, name, 'mat4', value.handle)
    elseif Vec:isInstance(value) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
        SetModelUniform(self.handle, name, 'vec'..#value, value:unpack())
    else
        assert(type, 'Type is missing.')
        SetModelUniform(self.handle, name, type, value)
    end
end

function Model:unsetUniform( name )
    UnsetModelUniform(self.handle, name)
end

function Model:_setTransformation( matrix )
    SetModelTransformation(self.handle, matrix.handle)
end

function Model:_setAttachmentTarget( solidHandle, flags )
    SetModelAttachmentTarget(self.handle, solidHandle, flags)
end


return Model
