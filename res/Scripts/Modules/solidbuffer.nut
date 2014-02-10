coord <- require("coord")
math <- require("math")

class SolidBuffer
{
    handle = null

    function constructor( buffer=null )
    {
        if(buffer)
        {
            if(type(buffer) == "userdata")
                handle = buffer
            else
                handle = buffer.handle
        }
        else
        {
            handle = ::native.blob.blob(0)
        }
    }

    function addSolid( aabb, tilePosition, userData, transformation=null )
    {
        if("min" in aabb && "max" in aabb)
        {
            aabb.halfWidth <- {
                x = (aabb.max.x - aabb.min.x) / 2.0,
                y = (aabb.max.y - aabb.min.y) / 2.0,
                z = (aabb.max.z - aabb.min.z) / 2.0
            }

            aabb.position <- {
                x = aabb.min.x + aabb.halfWidth.x,
                y = aabb.min.y + aabb.halfWidth.y,
                z = aabb.min.z + aabb.halfWidth.z
            }
        }

        if(transformation)
        {
            local originalAabb = aabb
            local rotation = transformation.toRotationMatrix()
            aabb = {}
            aabb.position <- transformation.transformVector(originalAabb.position),
            aabb.halfWidth <- rotation.transformVector(originalAabb.halfWidth)
            foreach(k,v in aabb.halfWidth)
                aabb.halfWidth[k] = math.abs(v)
        }

        tilePosition = coord.WorldToMapCoord(tilePosition)

        handle.writen(aabb.position.x, 'f') // f = float (32bit)
        handle.writen(aabb.position.y, 'f')
        handle.writen(aabb.position.z, 'f')
        handle.writen(aabb.halfWidth.x, 'f')
        handle.writen(aabb.halfWidth.y, 'f')
        handle.writen(aabb.halfWidth.z, 'f')

        handle.writen(tilePosition.x, 's') // s = short (16bit)
        handle.writen(tilePosition.z, 's')

        handle.writen(userData, 'i') // i = int (32bit)
    }
}

return SolidBuffer
