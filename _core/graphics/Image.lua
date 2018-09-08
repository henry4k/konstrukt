--- @classmod core.graphics.Image
--- Used internally to store image data in RAM.


local engine = require 'engine'
local class  = require 'middleclass'


local Image = class('core/graphics/Image')

function Image.static:load( fileName )
    return Image(engine.LoadImage(fileName), false)
end

function Image:initialize( handle, premultiplied )
    assert(type(handle) == 'userdata')

    self.handle = handle
    self.premultiplied = premultiplied

    local width, height, channelCount = engine.GetImageMetadata(handle)
    self.width = width
    self.height = height
    self.channelCount = channelCount
end

function Image:multiplyRgbByAlpha()
    if not self.premultiplied then
        engine.MultiplyImageRgbByAlpha(self.handle)
        self.premultiplied = true
    end
end

function Image:createResizedCopy( width, height )
    return Image(engine.CreateResizedImage(self.handle, width, height),
                 self.premultiplied)
end


return Image
