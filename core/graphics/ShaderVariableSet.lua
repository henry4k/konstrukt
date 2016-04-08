--- @classmod core.graphics.ShaderVariableSet


local engine  = require 'engine'
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'
local Mat4    = require 'core/Matrix4'
local Texture = require 'core/graphics/Texture'


local ShaderVariableSet = class('core/graphics/ShaderVariableSet')

function ShaderVariableSet:initialize( handle )
    assert(handle)
    self.handle = handle
    self.variables = {}
end

function ShaderVariableSet:destroy()
    self.handle = nil
end

function ShaderVariableSet:set( name, value )
    assert(self.handle)
    self.variables[name] = value
    if Object.isInstanceOf(value, Texture) then
        engine.SetTexture(self.handle, name, value.handle)
    elseif Object.isInstanceOf(value, Mat4) then
        engine.SetMat4Uniform(self.handle, name, value.handle)
    elseif Vec:isInstance(value) then
        assert(#value == 3, 'Only 3D vectors are supported.')
        engine.SetVec3Uniform(self.handle, name, value:unpack())
    else
        engine.SetFloatUniform(self.handle, name, value)
    end
end

function ShaderVariableSet:clear()
    engine.ClearShaderVariableSet(self.handle)
end


return ShaderVariableSet
