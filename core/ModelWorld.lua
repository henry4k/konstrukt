local class  = require 'middleclass'
local Model  = require 'core/Model'
local CreateModelWorld               = ENGINE.CreateModelWorld
local DestroyModelWorld              = ENGINE.DestroyModelWorld
local CreateModel                    = ENGINE.CreateModel


--- A model world defines a possibly large set of models that can be rendered together.
local ModelWorld = class('core/ModelWorld')

function ModelWorld:initialize()
    self.handle = CreateModelWorld()
end

function ModelWorld:destroy()
    DestroyModelWorld(self.handle)
    self.handle = nil
end

function ModelWorld:createModel()
    return Model(CreateModel(self.handle))
end


return ModelWorld
