local class = require 'core/middleclass'
local Vec   = require 'core/Vector'


--- Changes the velocity of a solid over time.
-- Here a force is defined by its vector and position. The vector defines the
-- Forces that are at the center will only change the solids linear velocity,
-- while forces that are applied off-center will also change the angular
-- velocity. They are applied in each simulation step until you 'destroy' them.
local Force = class('core/Force')

--- Initially all properties are zero, so that the force has no effect.
function Force:initialize( solidHandle )
    self.handle = NATIVE.CreateForce(solidHandle)
end

function Force:destroy()
    NATIVE.DestroyForce(self.handle)
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
function Force:set( value, relativePosition, useLocalCoordinates )
    relativePosition = relativePosition or Vec(0,0,0)
    NATIVE.SetForce(self.handle,
                    value[1],
                    value[2],
                    value[3],
                    relativePosition[1],
                    relativePosition[2],
                    relativePosition[3],
                    useLocalCoordinates)
end


return Force
