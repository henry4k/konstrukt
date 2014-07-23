package.cpath = ''
package.path = '?'

local fileSearcher = function( name )
    local path, message = package.searchpath(name, package.path, '/')
    if path then
        local loader = function( path ) return loadfile(path)() end
        return loader, path
    else
        return message
    end
end

package.searchers = { fileSearcher }

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)

------------------{ TEST }---------------------

local Shader        = require 'core/Shader.lua'
local ShaderProgram = require 'core/ShaderProgram.lua'
local Solid         = require 'core/Solid.lua'
local Matrix4       = require 'core/Matrix4.lua'
local ReferenceCube = require 'example/ReferenceCube/init.lua'

myProgram = ShaderProgram:new(
    Shader:new('core/Shaders/Test.vert'),
    Shader:new('core/Shaders/Test.frag'))

mySolid = Solid:new()

myCube = ReferenceCube:new(myProgram)
myCube.model:setTransformation(Matrix4:new():translate(0,0,4))
myCube.model:setAttachmentTarget(mySolid)
