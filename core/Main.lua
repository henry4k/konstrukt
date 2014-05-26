package.cpath = ''
package.path = './core/third-party/?.lua;./?.lua'

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)

---------------------------------------

local Mesh = require 'core/Mesh'
local MeshBuffer = require 'core/MeshBuffer'
local Shader = require 'core/Shader'
local ShaderProgram = require 'core/ShaderProgram'
local Model = require 'core/Model'

myProgram = ShaderProgram:new(
    Shader:new('core/Shaders/Test.vert'),
    Shader:new('core/Shaders/Test.frag'))

myMeshBuffer = MeshBuffer:new()
myMeshBuffer:appendIndex(0)
myMeshBuffer:appendIndex(1)
myMeshBuffer:appendIndex(2)

myMeshBuffer:appendVertex{x=0, y=0, z=3}
myMeshBuffer:appendVertex{x=0, y=1, z=3}
myMeshBuffer:appendVertex{x=1, y=1, z=3}

myMesh = Mesh:new(myMeshBuffer)

myModel = Model:new(myProgram)
myModel:setMesh(myMesh)
