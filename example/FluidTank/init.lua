local class = require 'core/middleclass'
local Model = require 'core/Model'
local Mesh  = require 'core/Mesh'
local Texture = require 'core/Texture'


local FluidTank = class('example/FluidTank')

function FluidTank:initialize( shaderProgram )
    local mesh = Mesh:load('example/FluidTank/Scene.json', 'Cylinder')
    local diffuseTexture = Texture:load('2d', 'example/FluidTank/Diffuse.png')
    self.model = Model(shaderProgram)
    self.model:setMesh(mesh)
    self.model:setTexture(0, diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
end

return FluidTank
