local class = require 'core/middleclass'
local Model = require 'core/Model'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'
local Mesh  = require 'core/Mesh'
local Texture = require 'core/Texture'
local ShaderProgram = require 'core/ShaderProgram'


local Skybox = class('example/Skybox')

function Skybox:initialize()
    local shaderProgram = ShaderProgram:load('example/Skybox/shader.vert',
                                             'example/Skybox/shader.frag')
    local mesh = Mesh:load('example/Skybox/Scene.json', 'Skybox')
    local diffuseTexture = Texture:load('cube', 'example/Skybox/%s.png')
    self.model = Model:new('background', shaderProgram)
    self.model:setMesh(mesh)
    self.model:setTexture(0, diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
    self.model:setTransformation(Mat4:new():scale(Vec:new(10,10,10)))
end

return Skybox
