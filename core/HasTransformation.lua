---
-- @module core.HasTransformation


local assert  = assert
local class   = require 'middleclass'
local Object  = class.Object
local Mat4    = require 'core/Matrix4'


local HasTransformation = {}

---
-- @param matrix
function HasTransformation:setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    self:_setTransformation(matrix)
end


return HasTransformation
