--- A model world defines a possibly large set of models that can be rendered together.
-- @module core.graphics.ModelWorld


local class  = require 'middleclass'
local Model  = require 'core/graphics/Model'
local CreateModelWorld  = ENGINE.CreateModelWorld
local DestroyModelWorld = ENGINE.DestroyModelWorld
local CreateModel       = ENGINE.CreateModel


local ModelWorld = class('core/graphics/ModelWorld')

function ModelWorld:initialize()
    self.handle = CreateModelWorld()
end

function ModelWorld:destroy()
    DestroyModelWorld(self.handle)
    self.handle = nil
end

--- Creates a new model in this model world.
-- @return[type=core.graphics.Model]
function ModelWorld:createModel()
    return Model(CreateModel(self.handle))
end


return ModelWorld
