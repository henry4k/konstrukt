local MeshBuffer = require 'core/MeshBuffer.lua'


local Scene = {}

function Scene.createMeshBuffer( definition )
    assert(definition.vertices)
    assert(definition.faces)

    local buffer = MeshBuffer:new()

    for _,vertex in ipairs(definition.vertices) do
        buffer:appendVertex(vertex)
    end

    for _,face in ipairs(definition.faces) do
        assert(#face == 3, 'Only triangles are supported!')
        for _,index in ipairs(face) do
            buffer:appendIndex(index)
        end
    end

    return buffer
end

return Scene
