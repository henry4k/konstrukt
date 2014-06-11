local MeshBuffer = require 'core/MeshBuffer'


local Scene = {}

function Scene.createMeshBuffer( definition )
    assert(definition.primitive == 'triangles',
           'Only triangles supported at the time.')
    assert(definition.indices)
    assert(definition.vertices)

    local buffer = MeshBuffer:new()

    for _,index in ipairs(definition.indices) do
        buffer:appendIndex(index)
    end

    for _,vertex in ipairs(definition.vertices) do
        buffer:appendVertex(vertex)
    end

    return buffer
end

return Scene
