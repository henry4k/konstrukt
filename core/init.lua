package.cpath = ''
package.path = 'core/third-party/?.lua;?.lua;?/init.lua'
-- TODO: Maybe use normal path names instead? => File name completion for free.

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)

------------------{ TEST }---------------------

local Shader        = require 'core/Shader'
local ShaderProgram = require 'core/ShaderProgram'
local Solid         = require 'core/Solid'
local Matrix4       = require 'core/Matrix4'
local ReferenceCube = require 'example/ReferenceCube'

myProgram = ShaderProgram:new(
    Shader:new('core/Shaders/Test.vert'),
    Shader:new('core/Shaders/Test.frag'))

mySolid = Solid:new()

myCube = ReferenceCube:new(myProgram)
myCube.model:setTransformation(Matrix4:new():translate(0,0,4))
myCube.model:setAttachmentTarget(mySolid)
