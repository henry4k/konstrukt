math <- require("math")
MeshBuffer <- require("meshbuffer")

function CreateMeshBufferFromTable( table )
{
    if(!math.TableContainsAllKeys(table, ["primitive", "vertex_format", "faces", "vertices"]))
        throw "Table is not a mesh."

    if(table.primitive != "triangles")
        throw "Mesh must consist of triangles."

    local buffer = MeshBuffer()

    local vertexCount = table.vertices.len()
    local faceCount = table.faces.len()

    local vertices = table.vertices
    local attribute = table.vertex_format.position
    for(local i = attribute.offset; i < vertexCount; i += attribute.component_count)
    {
        buffer.appendVertices([{
            x=vertices[i],
            y=vertices[i+1],
            y=vertices[i+2]
        }])
    }

    return buffer
}

return {
    CreateMeshBufferFromTable=CreateMeshBufferFromTable
}
