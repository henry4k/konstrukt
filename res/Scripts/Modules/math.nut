function Random( min, max )
{
    local rnd = ::native.math.rand().tofloat() / ::native.math.RAND_MAX.tofloat()
    local delta = max-min
    return min + rnd*delta
}

function RandomTableEntry( table )
{
    if(table.len() > 0)
        return table[ Random(0, table.len()-1).tointeger() ]
    else
        return null
}

class Matrix4
{
    m = null

    function constructor( matrix=null )
    {
        if(matrix)
        {
            if(type(matrix) == "userdata")
                m = matrix
            else
                m = matrix.m
        }
        else
        {
            m = ::native.CreateMatrix4()
        }
    }

    function _add( other )
    {
        return Matrix4( ::native.AddMatrix4(m, other.m) )
    }

    function _sub( other )
    {
        return Matrix4( ::native.SubMatrix4(m, other.m) )
    }

    function _mul( other )
    {
        return Matrix4( ::native.MulMatrix4(m, other.m) )
    }

    function _div( other )
    {
        return Matrix4( ::native.DivMatrix4(m, other.m) )
    }

    function translate( x,y,z )
    {
        return Matrix4( ::native.TranslateMatrix4(m, x.tofloat(), y.tofloat(), z.tofloat()) )
    }

    function scale( x,y,z )
    {
        return Matrix4( ::native.ScaleMatrix4(m, x.tofloat(), y.tofloat(), z.tofloat()) )
    }

    function rotate( x,y,z )
    {
        return Matrix4( ::native.RotateMatrix4(m, x.tofloat(), y.tofloat(), z.tofloat()) )
    }
}

return {
    "Random": Random,
    "RandomTableEntry": RandomTableEntry,
    "Matrix4": Matrix4
}.setdelegate(::native.math)
