math <- require("math")
json <- require("json")

class MeshBuffer
{
    handle = null

    function constructor( buffer=null )
    {
        if(buffer)
        {
            if(typeof(buffer) == "userdata")
                handle = buffer
            else
                handle = buffer.handle
        }
        else
        {
            handle = ::native.CreateMeshBuffer()
        }
    }

    function transform( matrix )
    {
        assert(matrix instanceof math.Matrix4)
        ::native.TransformMeshBuffer(handle, matrix.handle)
    }

    function appendMeshBuffer( buffer, transformation=null )
    {
        if(transformation instanceof math.Matrix4)
            ::native.AppendMeshBuffer(handle, buffer, transformation.handle)
        else
            ::native.AppendMeshBuffer(handle, buffer)
    }

    function appendIndices( indices )
    {
        local blob = ::native.blob.blob(0)
        foreach(index in indices)
            blob.writen(index, 'w') // unsigned short (16 bit)
        ::native.AppendIndicesToMeshBuffer(handle, blob)
    }

    function appendVertices( vertices )
    {
        local blob = ::native.blob.blob(0)

        local writeFloat = function( vertex, name, defaultValue )
        {
            local value = (name in vertex) ? vertex[name] : defaultValue
            blob.writen(value.tofloat(), 'f')
        }

        foreach(vertex in vertices)
        {
            writeFloat(vertex, "x", 0)
            writeFloat(vertex, "y", 0)
            writeFloat(vertex, "z", 0)
            writeFloat(vertex, "r", 1)
            writeFloat(vertex, "g", 1)
            writeFloat(vertex, "b", 1)
            writeFloat(vertex, "u", 0)
            writeFloat(vertex, "v", 0)
            writeFloat(vertex, "nx", 0)
            writeFloat(vertex, "ny", 0)
            writeFloat(vertex, "nz", 0)

            // Generate empty tangent vector
            blob.writen(0.0, 'f')
            blob.writen(0.0, 'f')
            blob.writen(0.0, 'f')
            blob.writen(0.0, 'f')
        }
        ::native.AppendVerticesToMeshBuffer(handle, blob)
    }
}

return MeshBuffer
