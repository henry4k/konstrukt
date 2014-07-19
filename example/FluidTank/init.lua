local class = require 'middleclass'

local Json    = require 'core/Json'
local Texture = require 'core/Texture'
local Mesh    = require 'core/Mesh'
local Scene   = require 'core/Scene'
local Model   = require 'core/Model'
local Matrix4 = require 'core/Matrix4'


local FluidTank = class('example/FluidTank')

function FluidTank:initialize( shaderProgram )

    self.shaderProgram = shaderProgram

    local scene = Json.decodeFromFile('example/FluidTank/Scene.json')
    local meshBuffer = Scene.createMeshBuffer(scene.Cylinder)
    self.mesh = Mesh:new(meshBuffer)

    self.diffuseTexture = Texture:new('2d', 'example/FluidTank/Diffuse.png')

    self.model = Model:new(self.shaderProgram)
    self.model:setMesh(self.mesh)
    self.model:setTexture(self.diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
end

return FluidTank
