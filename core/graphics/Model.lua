--- @classmod core.graphics.Model
--- Models are aggregations of meshes, textures, and shaders that can be used to render something on the screen.
--
-- Includes @{core.HasTransformation} and @{core.physics.HasAttachmentTarget}.


local engine  = require 'engine'
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'
local Mat4    = require 'core/Matrix4'
local Mesh    = require 'core/graphics/Mesh'
local Texture = require 'core/graphics/Texture'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'


local Model = class('core/graphics/Model')
Model:include(HasTransformation)
Model:include(HasAttachmentTarget)

--- Can't be instanciated directly, use @{core.graphics.ModelWorld:createModel} instead.
function Model:initialize()
    assert(self.handle, 'Can\'t be instanciated directly, use ModelWorld:createModel instead.')
    self.attachmentTarget = nil
    self.mesh = nil
    self.activeTextures = {}
    self.activeUniforms = {}
end

function Model:destroy()
    engine.DestroyModel(self.handle)
    self.handle = nil
end

--- Changes a models mesh.
function Model:setMesh( mesh )
    assert(Object.isInstanceOf(mesh, Mesh), 'Must be called with a mesh.')
    engine.SetModelMesh(self.handle, mesh.handle)
    self.mesh = mesh
end

--- Retrieves the currently used mesh.
function Model:getMesh()
    return self.mesh
end

function Model:setOverlayLevel( level )
    assert(math.isInteger(level), 'Overlay level must be an integer.')
    engine.SetModelOverlayLevel(self.handle, level)
end

--- Changes a texture unit.
-- @param[type=number] unit
-- @param[type=core.graphics.Texture] texture
function Model:setTexture( unit, texture )
    assert(math.isInteger(unit), 'Unit must be an integer.')
    assert(unit >= 0, 'Unit must be positive.')
    assert(Object.isInstanceOf(texture, Texture), 'Must be called with a texture.')
    engine.SetModelTexture(self.handle, unit, texture.handle)
    self.activeTextures[unit] = true
end

function Model:unsetAllTextures()
    for unit, _ in pairs(self.activeTextures) do
        engine.SetModelTexture(self.handle, unit, nil)
        self.activeTextures[unit] = nil
    end
end

--- Changes the programs family.
--
-- @param[type=string] family
--
-- @param[type=string] ...
-- Fallback families, which are tried if a @{core.graphics.ShaderProgramSet} lacks the
-- originally requested family.
--
function Model:setProgramFamily( family, ... )
    local families = {family, ...}
    engine.SetModelProgramFamilyList(self.handle, table.concat(families, ','))
end

--- Changes a model specific uniform.
--
-- @param name
-- @param[type=number|Vector|Matrix4] value
-- @param[opt] type
-- Is needed for number unifroms.  Either `int` or `float` are applicable.
--
function Model:setUniform( name, value, type )
    if Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
        engine.SetModelUniform(self.handle, name, 'mat4', value.handle)
    elseif Vec:isInstance(value) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
        engine.SetModelUniform(self.handle, name, 'vec'..#value, value:unpack())
    else
        assert(type, 'Type is missing.')
        engine.SetModelUniform(self.handle, name, type, value)
    end
    self.activeUniforms[name] = true
end

--- Remove a uniform.
function Model:unsetUniform( name )
    engine.UnsetModelUniform(self.handle, name)
    self.activeUniforms[name] = nil
end

function Model:unsetAllUniforms()
    for name, _ in pairs(self.activeUniforms) do
        self:unsetUniform(name)
    end
end

function Model:_setTransformation( matrix )
    engine.SetModelTransformation(self.handle, matrix.handle)
end

function Model:_setAttachmentTarget( solidHandle, flags )
    engine.SetModelAttachmentTarget(self.handle, solidHandle, flags)
end


return Model
