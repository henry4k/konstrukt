local assert  = assert
local class   = require 'middleclass'
local Object  = class.Object
local Mat4    = require 'core/Matrix4'


local HasTransformation = { static = {} }

function HasTransformation:included( targetClass, setTransformationFn )
    assert(setTransformationFn, 'Missing function to set transformation.')
    targetClass.setTransformation_ = setTransformationFn
end

function HasTransformation:setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    self:setTransformation_(self, matrix)
end


return Transformation
