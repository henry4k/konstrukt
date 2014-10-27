local assert  = assert
local class   = require 'middleclass'
local Object  = class.Object
local Mat4    = require 'core/Matrix4'


local HasTransformation = {}

function HasTransformation:setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    self:_setTransformation(self, matrix)
end


return HasTransformation
