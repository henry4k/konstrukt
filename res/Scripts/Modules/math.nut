function Random( min=0.0, max=1.0 )
{
    local rnd = ::native.math.rand().tofloat() / ::native.math.RAND_MAX.tofloat()
    local delta = max-min
    return min + rnd*delta
}

function RandomArrayElement( array )
{
    if(array.len() > 0)
        return array[ Random(0, array.len()-1+0.5).tointeger() ]
    else
        return null
}

class Matrix4
{
    handle = null

    function constructor( matrix=null )
    {
        if(matrix)
        {
            if(type(matrix) == "userdata")
            {
                handle = matrix
            }
            else
            {
                assert(matrix instanceof Matrix4)
                handle = ::native.CopyMatrix4(matrix.handle)
            }
        }
        else
        {
            handle = ::native.CreateMatrix4()
        }
    }

    function _add( other )
    {
        assert(other instanceof Matrix4)
        return Matrix4( ::native.AddMatrix4(handle, other.handle) )
    }

    function _sub( other )
    {
        assert(other instanceof Matrix4)
        return Matrix4( ::native.SubMatrix4(handle, other.handle) )
    }

    function _mul( other )
    {
        assert(other instanceof Matrix4)
        return Matrix4( ::native.MulMatrix4(handle, other.handle) )
    }

    function _div( other )
    {
        assert(other instanceof Matrix4)
        return Matrix4( ::native.DivMatrix4(handle, other.handle) )
    }

    function translate( x,y,z )
    {
        return Matrix4( ::native.TranslateMatrix4(handle, x.tofloat(), y.tofloat(), z.tofloat()) )
    }

    function scale( x,y,z )
    {
        return Matrix4( ::native.ScaleMatrix4(handle, x.tofloat(), y.tofloat(), z.tofloat()) )
    }

    function rotate( angle, x,y,z )
    {
        return Matrix4( ::native.RotateMatrix4(handle, angle.tofloat(), x.tofloat(), y.tofloat(), z.tofloat()) )
    }

    function transformVector( v )
    {
        local r = ::native.Matrix4TransformVector(handle, v.x.tofloat(), v.y.tofloat(), v.z.tofloat(), 1.0)
        return {
            x = r[0],
            y = r[1],
            z = r[2]
        }
    }

    function toRotationMatrix()
    {
        return Matrix4( ::native.MakeRotationMatrix(handle) )
    }
}

/**
 * Since the sqstdlib abs is only for integers!
 */
function abs( v )
{
    return (v < 0) ? -v : v;
}

function TableContainsAllKeys( table, keys )
{
    foreach(key in keys)
        if(!key in table)
            return false
    return true
}

function TableContainsAnyKeys( table, keys )
{
    foreach(key in keys)
        if(key in table)
            return true
    return false
}

return {
    Random = Random,
    RandomArrayElement = RandomArrayElement,
    Matrix4 = Matrix4,
    abs = abs
}.setdelegate(::native.math)
