local class = require 'core/middleclass.lua'

local Json    = require 'core/Json.lua'
local Texture = require 'core/Texture.lua'
local Mesh    = require 'core/Mesh.lua'
local Scene   = require 'core/Scene.lua'
local Model   = require 'core/Model.lua'
local Matrix4 = require 'core/Matrix4.lua'


local ReferenceCube = class('example/ReferenceCube')

function ReferenceCube:initialize( shaderProgram )

    self.shaderProgram = shaderProgram

    local scene = Json.decodeFromFile('example/ReferenceCube/Scene.json')
    local meshBuffer = Scene.createMeshBuffer(scene.Cube)
    self.mesh = Mesh:new(meshBuffer)

    self.diffuseTexture = Texture:new('2d', 'example/ReferenceCube/Diffuse.png')

    self.model = Model:new(self.shaderProgram)
    self.model:setMesh(self.mesh)
    self.model:setTexture(self.diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
end

return ReferenceCube
