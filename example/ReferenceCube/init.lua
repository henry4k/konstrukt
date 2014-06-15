local class = require 'middleclass'

local Json    = require 'core/Json'
local Texture = require 'core/Texture'
local Mesh    = require 'core/Mesh'
local Scene   = require 'core/Scene'
local Model   = require 'core/Model'
local Matrix4 = require 'core/Matrix4'


local ReferenceCube = class('example/ReferenceCube')

function ReferenceCube:initialize( shaderProgram )

    self.shaderProgram = shaderProgram

    local scene = Json.decodeFromFile('example/ReferenceCube/Scene.json')
    local meshBuffer = Scene.createMeshBuffer(scene.Cube)
    local transformation = Matrix4:new()
    print(transformation)
    transformation = transformation:translate(0,0,2)
    meshBuffer:transform(transformation)
    self.mesh = Mesh:new(meshBuffer)

    self.diffuseTexture = Texture:new('2d', 'example/ReferenceCube/Diffuse.png')

    self.model = Model:new(self.shaderProgram)
    self.model:setMesh(self.mesh)
    self.model:setTexture(self.diffuseTexture)
    self.model:setUniform('DiffuseSampler', 0)
end

return ReferenceCube
