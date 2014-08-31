local class = require 'core/middleclass'

local Json    = require 'core/Json'
local Texture = require 'core/Texture'
local Mesh    = require 'core/Mesh'
local Scene   = require 'core/Scene'
local Model   = require 'core/Model'
local Vec     = require 'core/Vector'
local Mat4    = require 'core/Matrix4'
local Shader  = require 'core/Shader'
local ShaderProgram = require 'core/ShaderProgram'


local Skybox = class('example/Skybox')

function Skybox:initialize()
    local shaderProgram = ShaderProgram:new(
        Shader:new('example/Skybox/shader.vert'),
        Shader:new('example/Skybox/shader.frag'))

    local scene = Json.decodeFromFile('example/Skybox/Scene.json')
    local meshBuffer = Scene.createMeshBuffer(scene.Skybox)
    local mesh = Mesh:new(meshBuffer)

    local diffuseTexture = Texture:new('cube', 'example/Skybox/%s.png') --, 'filter')

    self.model = Model:new('background', shaderProgram)
    self.model:setMesh(mesh)
    self.model:setTexture(0, diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
    self.model:setTransformation(Mat4:new():scale(Vec:new(10,10,10)))
end

return Skybox
