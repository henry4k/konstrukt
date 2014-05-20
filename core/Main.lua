package.cpath = ''
package.path = './?.lua'

Mesh = require 'core/Mesh'
MeshBuffer = require 'core/MeshBuffer'
GraphicsObject = require 'core/GraphicsObject'

myMeshBuffer = MeshBuffer.new()
myMeshBuffer:appendIndex(0)
myMeshBuffer:appendIndex(1)
myMeshBuffer:appendIndex(2)

myMeshBuffer:appendVertex{x=0, y=0, z=3}
myMeshBuffer:appendVertex{x=0, y=1, z=3}
myMeshBuffer:appendVertex{x=1, y=1, z=3}

myMesh = Mesh.new(myMeshBuffer)

myGraphicsObject = GraphicsObject.new()
myGraphicsObject:setMesh(myMesh)
