math <- require("math")

class MeshBuffer
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
            handle = ::native.CreateMeshBuffer()
        }
    }

    function buildMesh()
    {
        return ::native.BuildMesh(handle)
    }

    function transform( matrix )
    {
        assert(matrix instanceof math.Matrix4)
        ::native.TransformMeshBuffer(handle, matrix.handle)
    }

    function addMesh( mesh, transformation=null )
    {
        if(transformation instanceof math.Matrix4)
            ::native.AddMeshToMeshBuffer(handle, mesh, transformation.handle)
        else
            ::native.AddMeshToMeshBuffer(handle, mesh)
    }

    function addMeshBuffer( meshBuffer, transformation=null )
    {
        if(transformation instanceof math.Matrix4)
            ::native.AddMeshBufferToMeshBuffer(handle, meshBuffer.handle, transformation)
        else
            ::native.AddMeshBufferToMeshBuffer(handle, meshBuffer.handle)
    }
}

return MeshBuffer
