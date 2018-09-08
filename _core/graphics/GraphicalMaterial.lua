local class     = require 'middleclass'
local Object    = class.Object
local Vec       = require 'core/Vector'
local Mat4      = require 'core/Matrix4'
local Texture   = require 'core/graphics/Texture'
local Model     = require 'core/graphics/Model'


local Material = class('core/graphics/GraphicalMaterial')

function Material:initialize()
    self.overlayLevel = 0
    self.programFamilyList = nil
    self.shaderVariables = {}
end

function Material:setOverlayLevel( level )
    assert(math.isInteger(level), 'Overlay level must be an integer.')
    self.overlayLevel = level
end

function Material:setProgramFamily( family, ... )
    self.programFamilyList = {family, ...}
end

function Material:updateModel( model )
    assert(Object.isInstanceOf(model, Model), 'Must be called with a model.')

    model:setOverlayLevel(self.overlayLevel)

    model:setProgramFamily(table.unpack(self.programFamilyList))

    model.shaderVariables:clear()
    for name, value in pairs(self.shaderVariables) do
        model.shaderVariables:set(name, value)
    end
end


return Material
