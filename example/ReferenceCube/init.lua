local class = require 'core/middleclass'

local Json    = require 'core/Json'
local Texture = require 'core/Texture'
local Mesh    = require 'core/Mesh'
local Scene   = require 'core/Scene'
local Model   = require 'core/Model'
local Matrix4 = require 'core/Matrix4'


local ReferenceCube = class('example/ReferenceCube')

function ReferenceCube:initialize( stage, shaderProgram )

    local scene = Json.decodeFromFile('example/ReferenceCube/Scene.json')
    local meshBuffer = Scene.createMeshBuffer(scene.Cube)
    local mesh = Mesh:new(meshBuffer)

    local diffuseTexture = Texture:new('2d', 'example/ReferenceCube/Diffuse.png')

    self.model = Model:new(stage, shaderProgram)
    self.model:setMesh(mesh)
    self.model:setTexture(0, diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
end

return ReferenceCube
