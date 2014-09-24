local MeshBuffer = require 'apoapsis.core.MeshBuffer'


local GetEntryByPath = function( table, path, delimiters )
    local delimiters = delimiters or '.'
    local current = table
    for entry in path:gmatch('[^'..delimiters..']+') do
        if current[entry] then
            current = current[entry]
        else
            return nil
        end
    end
    return current
end


local Scene = {}

function Scene.createMeshBuffer( definition )
    assert(definition.vertices)
    assert(definition.faces)

    local buffer = MeshBuffer()

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

function Scene.createMeshBufferByPath( sceneGraph, objectName )
    local definition = GetEntryByPath(sceneGraph, objectName, './')
    return Scene.createMeshBuffer(definition)
end


return Scene
