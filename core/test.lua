package.searchers = { fileSearcher }

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)

------------------{ TEST }---------------------

local Vec           = require 'core/Vector'
local Quat          = require 'core/Quaternion'
local Mat4          = require 'core/Matrix4'
local Shader        = require 'core/Shader'
local ShaderProgram = require 'core/ShaderProgram'
local BoxCollisionShape = require 'core/collision_shapes/BoxCollisionShape'
local Solid         = require 'core/Solid'
local ReferenceCube = require 'example/ReferenceCube/init'

myProgram = ShaderProgram:new(
    Shader:new('core/Shaders/Test.vert'),
    Shader:new('core/Shaders/Test.frag'))

myShape = BoxCollisionShape:new(Vec:new(0.5, 0.5, 0.5))
mySolid = Solid:new(1,
                    Vec:new(0,0,0),
                    Quat:new(),
                    myShape)

myCube = ReferenceCube:new(myProgram)
myCube.model:setTransformation(Matrix4:new():translate(Vec:new(0,0,4)))
myCube.model:setAttachmentTarget(mySolid)
