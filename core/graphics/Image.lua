--- @classmod core.graphics.Image
--- Used internally to store image data in RAM.


local class    = require 'middleclass'
local LoadImage = ENGINE.LoadImage
local MultiplyImageRgbByAlpha = ENGINE.MultiplyImageRgbByAlpha
local CreateResizedImage = ENGINE.CreateResizedImage
local GetImageMetadata = ENGINE.GetImageMetadata


local Image = class('core/graphics/Image')

function Image.static:load( fileName )
    return Image(LoadImage(fileName), false)
end

function Image:initialize( handle, premultiplied )
    assert(type(handle) == 'userdata')

    self.handle = handle
    self.premultiplied = premultiplied

    local width, height, channelCount = GetImageMetadata(handle)
    self.width = width
    self.height = height
    self.channelCount = channelCount
end

function Image:multiplyRgbByAlpha()
    if not self.premultiplied then
        MultiplyImageRgbByAlpha(self.handle)
        self.premultiplied = true
    end
end

function Image:createResizedCopy( width, height )
    return Image(CreateResizedImage(self.handle, width, height),
                 self.premultiplied)
end


return Image
