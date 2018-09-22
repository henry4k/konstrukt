--- @classmod core.graphics.Model
--- Models are aggregations of meshes, textures, and shaders that can be used to render something on the screen.
--
-- Includes @{core.HasTransformation} and @{core.physics.HasAttachmentTarget}.


local engine    = require 'engine'
local class     = require 'middleclass'
local Object    = class.Object
local Scheduler = require 'core/Scheduler'
local Mesh      = require 'core/graphics/Mesh'
local ShaderVariableSet   = require 'core/graphics/ShaderVariableSet'
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
    self.shaderVariables =
        ShaderVariableSet(Scheduler.awaitCall(engine.GetModelShaderVariableSet, self.handle))
end

function Model:destroy()
    Scheduler.blindCall(engine.DestroyModel, self.handle)
    self.handle = nil
    self.shaderVariables:destroy()
    self.shaderVariables = nil
end

--- Changes a models mesh.
function Model:setMesh( mesh )
    assert(Object.isInstanceOf(mesh, Mesh), 'Must be called with a mesh.')
    Scheduler.blindCall(engine.SetModelMesh, self.handle, mesh.handle)
    self.mesh = mesh
end

--- Retrieves the currently used mesh.
function Model:getMesh()
    return self.mesh
end

function Model:setOverlayLevel( level )
    assert(math.isInteger(level), 'Overlay level must be an integer.')
    Scheduler.blindCall(engine.SetModelOverlayLevel, self.handle, level)
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
    Scheduler.blindCall(engine.SetModelProgramFamilyList, self.handle, table.concat(families, ','))
end

function Model:_setTransformation( matrix )
    Scheduler.blindCall(engine.SetModelTransformation, self.handle, matrix.handle)
end

function Model:_setAttachmentTarget( solidHandle, flags )
    Scheduler.blindCall(engine.SetModelAttachmentTarget, self.handle, solidHandle, flags)
end


return Model
