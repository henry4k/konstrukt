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

    function addAabb( aabb, transformation=null )
    {
        if(transformation)
        {
            local originalAabb = aabb
            local rotation = transformation.toRotationMatrix()
            aabb = {}
            aabb.position <- transformation.transformVector(originalAabb.position),
            aabb.halfWidth <- rotation.transformVector(originalAabb.halfWidth)
        }

        handle.writen(aabb.position.x, 'f')
        handle.writen(aabb.position.y, 'f')
        handle.writen(aabb.position.z, 'f')
        handle.writen(aabb.halfWidth.x, 'f')
        handle.writen(aabb.halfWidth.y, 'f')
        handle.writen(aabb.halfWidth.z, 'f')
    }
}

return SolidBuffer
