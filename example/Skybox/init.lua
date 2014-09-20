local class = require 'core/middleclass'
local Model = require 'core/Model'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'
local Mesh  = require 'core/Mesh'
local Texture = require 'core/Texture'
local ShaderProgram = require 'core/ShaderProgram'


local Skybox = class('example/Skybox')

function Skybox:initialize( modelWorld )
    local shaderProgram = ShaderProgram:load('example/Skybox/shader.vert',
                                             'example/Skybox/shader.frag')
    local mesh = Mesh:load('example/Skybox/Scene.json', 'Skybox')
    local diffuseTexture = Texture:load('cube', 'example/Skybox/%s.png')
    self.model = modelWorld:createModel()
    self.model:setMesh(mesh)
    self.model:setRenderLayer(-1)
    self.model:setProgramFamilyList('skybox')
    self.model:setTexture(0, diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
    self.model:setTransformation(Mat4():scale(Vec(10,10,10)))
end

return Skybox
