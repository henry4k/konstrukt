--- @module core.graphics.ModelWorld
--- A model world defines a possibly large set of models that can be rendered together.


local engine = require 'engine'
local class  = require 'middleclass'
local Model  = require 'core/graphics/Model'


local ModelWorld = class('core/graphics/ModelWorld')

function ModelWorld:initialize()
    self.handle = engine.CreateModelWorld()
end

function ModelWorld:destroy()
    engine.DestroyModelWorld(self.handle)
    self.handle = nil
end

--- Creates a new model in this model world.
-- @return[type=core.graphics.Model]
function ModelWorld:createModel()
    return Model(engine.CreateModel(self.handle))
end


return ModelWorld
