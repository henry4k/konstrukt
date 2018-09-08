--- @classmod core.physics.Force
--- Changes the velocity of a @{core.physics.Solid} over time.
--
-- Here a force is defined by its vector and position. The vector defines the
-- Forces that are at the center will only change the solids linear velocity,
-- while forces that are applied off-center will also change the angular
-- velocity. They are applied in each simulation step until you 'destroy' them.


local engine = require 'engine'
local class  = require 'middleclass'
local Vec    = require 'core/Vector'


local Force = class('core/physics/Force')

--- *DON'T CALL THIS DIRECTLY!*  Use @{core.physics.Solid:createForce} instead.
function Force:initialize( handle )
    assert(type(handle) == 'userdata', 'Create forces with Solid:createForce()!')
    self.handle = handle
end

function Force:destroy()
    engine.DestroyForce(self.handle)
    self.handle = nil
end

--- Changes the properties of the force.
--
-- @param value
-- Describes the magnitude and direction that is applied in one second.
--
-- @param relativePosition
-- Point where the force is applied to the solid.
--
-- @param useLocalCoordinates
-- If set direction and position will be relative to the solids orientation.
--
function Force:set( value, relativePosition, useLocalCoordinates )
    assert(Vec:isInstance(value), 'Value must be vector.')
    relativePosition    = relativePosition or Vec(0,0,0)
    assert(Vec:isInstance(relativePosition), 'Relative position must be vector.')
    useLocalCoordinates = useLocalCoordinates or false
    engine.SetForce(self.handle,
                    value[1],
                    value[2],
                    value[3],
                    relativePosition[1],
                    relativePosition[2],
                    relativePosition[3],
                    useLocalCoordinates)
end


return Force
