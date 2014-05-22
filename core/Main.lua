package.cpath = ''
package.path = './core/third-party/?.lua;./?.lua'

Mesh = require 'core/Mesh'
MeshBuffer = require 'core/MeshBuffer'
Model = require 'core/Model'

myMeshBuffer = MeshBuffer.new()
myMeshBuffer:appendIndex(0)
myMeshBuffer:appendIndex(1)
myMeshBuffer:appendIndex(2)

myMeshBuffer:appendVertex{x=0, y=0, z=3}
myMeshBuffer:appendVertex{x=0, y=1, z=3}
myMeshBuffer:appendVertex{x=1, y=1, z=3}

myMesh = Mesh.new(myMeshBuffer)

myModel = Model.new()
myModel:setMesh(myMesh)
