local class     = require 'middleclass'
local Object    = class.Object
local Vec       = require 'core/Vector'
local Mat4      = require 'core/Matrix4'
local Texture   = require 'core/graphics/Texture'
local Model     = require 'core/graphics/Model'


local Material = class('core/graphics/GraphicalMaterial')

function Material:initialize()
    self.overlayLevel = 0
    self.textures = {}
    self.programFamilyList = nil
    self.uniforms = {}
    self.shaderVariables = {}
end

function Material:setOverlayLevel( level )
    assert(math.isInteger(level), 'Overlay level must be an integer.')
    self.overlayLevel = level
end

function Material:setTexture( unit, texture )
    assert(math.isInteger(unit), 'Unit must be an integer.')
    assert(unit >= 0, 'Unit must be positive.')
    assert(Object.isInstanceOf(texture, Texture), 'Must be called with a texture.')
    self.textures[unit] = texture
end

function Material:setProgramFamily( family, ... )
    self.programFamilyList = {family, ...}
end

function Material:setUniform( name, value, type )
    if Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
    elseif Vec:isInstance(value) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
    else
        assert(type, 'Type is missing.')
    end
    self.uniforms[name] = { value=value, type=type }
end

function Material:updateModel( model )
    assert(Object.isInstanceOf(model, Model), 'Must be called with a model.')

    model:setOverlayLevel(self.overlayLevel)

    model:unsetAllTextures()
    for unit, texture in pairs(self.textures) do
        model:setTexture(unit, texture)
    end

    model:setProgramFamily(table.unpack(self.programFamilyList))

    model:unsetAllUniforms()
    for name, uniform in pairs(self.uniforms) do
        model:setUniform(name, uniform.value, uniform.type)
    end

    for name, value in pairs(self.shaderVariables) do
        model.shaderVariables:set(name, value)
    end
end


return Material
