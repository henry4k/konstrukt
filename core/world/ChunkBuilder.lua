local assert            = assert
local class             = require 'middleclass'
local Object            = class.Object
local Mesh              = require 'core/graphics/Mesh'
local MeshBuffer        = require 'core/graphics/MeshBuffer'
local GraphicalMaterial = require 'core/graphics/GraphicalMaterial'


--- Helper class that is used to update the static models and solids of a chunk.
local ChunkBuilder = class('core/world/ChunkBuilder')

function ChunkBuilder:initialize()
    self.meshBuffers = {}
end

function ChunkBuilder:destroy()
    for _, meshBuffer in pairs(self.meshBuffers) do
        meshBuffer:destroy()
    end
end

function ChunkBuilder:addMeshBuffer( material, newGeometry, transformation )
    assert(Object.isInstanceOf(material, GraphicalMaterial),
        'Must be called with a graphical material.')
    assert(Object.isInstanceOf(newGeometry, MeshBuffer),
        'Must be called with a mesh buffer.')

    local meshBuffer = self.meshBuffers[material]
    if not meshBuffer then
        meshBuffer = MeshBuffer()
        self.meshBuffers[material] = meshBuffer
    end

    meshBuffer:appendMeshBuffer(newGeometry, transformation)
end

function ChunkBuilder:updateModels( models, modelWorld )
    local meshBuffers = self.meshBuffers

    -- Create or update models:
    for material, meshBuffer in pairs(meshBuffers) do
        local model = models[material]
        if not model then
            model = modelWorld:createModel()
            material:updateModel(model)
            models[material] = model
        end
        model:setMesh(Mesh(meshBuffer))
    end

    -- Destroy unused models:
    for material, model in pairs(models) do
        local meshBuffer = meshBuffers[material]
        if not meshBuffer then
            model:destroy()
            models[material] = nil
        end
    end
end


return ChunkBuilder
