--- @classmod core.graphics.Image
--- Used internally to store image data in RAM.


local engine    = require 'engine'
local class     = require 'middleclass'
local Scheduler = require 'core/Scheduler'


local Image = class('core/graphics/Image')

function Image.static:load( fileName )
    return Image(Scheduler.awaitCall(engine.LoadImage, fileName), false)
end

function Image:initialize( handle, premultiplied )
    assert(type(handle) == 'userdata')

    self.handle = handle
    self.premultiplied = premultiplied

    local width, height, channelCount = Scheduler.awaitCall(engine.GetImageMetadata, handle)
    self.width = width
    self.height = height
    self.channelCount = channelCount
end

function Image:multiplyRgbByAlpha()
    if not self.premultiplied then
        Scheduler.awaitCall(engine.MultiplyImageRgbByAlpha, self.handle)
        self.premultiplied = true
    end
end

function Image:createResizedCopy( width, height )
    return Image(Scheduler.awaitCall(engine.CreateResizedImage, self.handle, width, height),
                 self.premultiplied)
end


return Image
