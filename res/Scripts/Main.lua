package.cpath = ''
package.path = './Scripts/Modules/?.lua'

mesh = require 'mesh'
meshBuffer = require 'meshBuffer'
graphicsObject = require 'graphicsObject'

myMeshBuffer = meshBuffer.new()
myMeshBuffer:appendIndex(0)
myMeshBuffer:appendIndex(1)
myMeshBuffer:appendIndex(2)

myMeshBuffer:appendVertex{x=0, y=0, z=3}
myMeshBuffer:appendVertex{x=0, y=1, z=3}
myMeshBuffer:appendVertex{x=1, y=1, z=3}

myMesh = mesh.new(myMeshBuffer)

myGraphicsObject = graphicsObject.new()
myGraphicsObject:setMesh(myMesh)
