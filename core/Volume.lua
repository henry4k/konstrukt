local class   = require 'middleclass'
local Vec     = require 'core/Vector'


local Volume = class('core/Volume')

function Volume:initialize()
end

function Volume:isInside( position )
    assert(Vec:isInstance(position), 'Must be called with a vector.')
    return false
end


return Volume
