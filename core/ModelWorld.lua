local class  = require 'middleclass'
local Model  = require 'core/Model'
local CreateModelWorld               = ENGINE.CreateModelWorld
local SetRenderLayerNearAndFarPlanes = ENGINE.SetRenderLayerNearAndFarPlanes
local DestroyModelWorld              = ENGINE.DestroyModelWorld
local CreateModel                    = ENGINE.CreateModel


--- A model world defines a possibly large set of models that can be rendered together.
local ModelWorld = class('core/ModelWorld')

--- @class RenderLayer
-- When rendering the model world, the models are grouped into layers.
--
-- Models from a lower layer can't occlude models from higher layers.
-- Also each layer has own z near and z far planes.
--
-- Render layers are used to separate HUD and background from the regular
-- scene.

-- ModelWorld({{name=foo, zNear=1, zFar=2},
--             {name=bar, zNear=1, zFar=2}})
function ModelWorld:initialize( renderLayers )
    self.handle = CreateModelWorld()

    renderLayers = renderLayers or {{name='default'}}
    self.renderLayersByName = {}
    for i,definition in ipairs(renderLayers) do
        local layerIndex = i-1
        SetRenderLayerNearAndFarPlanes(self.handle,
                                              layerIndex,
                                              definition.zNear or   0.1,
                                              definition.zFar  or 100.0)
        self.renderLayersByName[definition.name] = layerIndex
    end
end

function ModelWorld:destroy()
    DestroyModelWorld(self.handle)
    self.handle = nil
end

function ModelWorld:getRenderLayerIndex_( name )
    local index = self.renderLayersByName[name]
    if index then
        return index
    else
        error('Render layer "'..name..'" not known in '..self..'.')
    end
end

function ModelWorld:setRenderLayerNearAndFarPlanes( layerName, zNear, zFar )
    local layerIndex = self:getRenderLayerIndex_(layerName)
end

function ModelWorld:createModel( renderLayerName )
    renderLayerName = renderLayerName or 'default'
    local renderLayerIndex = self:getRenderLayerIndex_(renderLayerName)
    return Model(CreateModel(self.handle, renderLayerIndex),
                                    renderLayerName)
end


return ModelWorld
