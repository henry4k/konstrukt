local class = require 'core/middleclass'
local Model = require 'core/Model'


local ModelWorld = class('core/ModelWorld')

function ModelWorld:initialize()
    self.handle = NATIVE.CreateModelWorld()
end

function ModelWorld:destroy()
    NATIVE.DestroyModelWorld(self.handle)
    self.handle = nil
end

function ModelWorld:createModel()
    return Model(NATIVE.CreateModel(self.handle))
end


return ModelWorld