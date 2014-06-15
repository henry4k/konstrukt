package.cpath = ''
package.path = 'core/third-party/?.lua;?.lua;?/init.lua'

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)

------------------{ TEST }---------------------

local Shader = require 'core/Shader'
local ShaderProgram = require 'core/ShaderProgram'
myProgram = ShaderProgram:new(
    Shader:new('core/Shaders/Test.vert'),
    Shader:new('core/Shaders/Test.frag'))

local ReferenceCube = require 'example/ReferenceCube'
myCube = ReferenceCube:new(myProgram)
