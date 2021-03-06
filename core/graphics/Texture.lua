--- @classmod core.graphics.Texture
--- Image, which can be directly used for rendering by the graphics card.
--
-- Includes @{core.Resource}.


local engine   = require 'engine'
local class    = require 'middleclass'
local Scheduler = require'core/Scheduler'
local Resource = require 'core/Resource'
local Config   = require 'core/Config'
local Image    = require 'core/graphics/Image'


local Texture = class('core/graphics/Texture')
Texture:include(Resource)

--- Creates a texture from a file.
--
-- @function static:load( target, fileName, flags )
--
-- @param[type=string] target
-- Texture type.  Available are:
--
-- - ` 2d`:  Regular 2d images.
-- - `cube`:  Loads a cube map.  The file name is used as a template.
--   It must contain one `%s`, which is used to expand the template to six paths:
--   One for `px`, `nx`, `py`, `ny`, `pz`, and `nz`.
--
-- @param fileName
--
-- @param[type=boolean] filter
-- How texture samples are accessed.  See https://www.opengl.org/wiki/Sampler_Object#Filtering
--
-- Available are:
--
-- - `nearest`:
-- - `linear`:
--
-- Defaults to **linear**.
--
-- @param[type=string] wrapMode
-- How texture sampling outside the coordinate range from 0 to 1 behaves.
--
-- Available are:
--
-- - `repeat`:
-- - `clamp`:  Limit texture coordinates to a range between 0 and 1.
--
-- Defaults to **repeat**.
--
-- @param[type=string] colorSpace
-- Which color space has been used to encode the pixel values.
--
-- Available are:
--
-- - `linear`
-- - `srgb`
--
-- Images whose pixel values don't describe a color,
-- should always use the linear color space.
-- E.g. normal and metallic maps
--
-- Defaults to **linear**.
--
function Texture.static:_load( options )
    local texture = Texture(options)
    return { value=texture, destructor=texture.destroy }
end

local cubeMapSides = { 'px', 'nx', 'py', 'ny', 'pz', 'nz' }

Texture.static.defaultOptions =
{
    target = '2d',
    filter = 'linear',
    wrapMode = 'repeat',
    mipmap = true,
    colorSpace = 'linear',
    multiplyRgbByAlpha = true
}

function Texture:initialize( options )
    setmetatable(options, { __index=Texture.defaultOptions })

    local flags = {}
    if options.mipmap == true then
        table.insert(flags, 'mipmap')
    end
    if options.filter == 'linear' then
        table.insert(flags, 'filter')
    end
    if options.wrapMode == 'clamp' then
        table.insert(flags, 'clamp')
    end
    if options.colorSpace == 'srgb' then
        table.insert(flags, 'srgb')
    else
        assert(options.colorSpace == 'linear')
    end

    if options.target == '2d' then
        local image = self:_loadImage(options.fileName, options)
        self.handle = Scheduler.awaitCall(engine.Create2dTexture, image.handle, table.unpack(flags))
    elseif options.target == 'cube' then
        local images = {}
        for i,v in ipairs(cubeMapSides) do
            local image = self:_loadImage(string.format(options.fileName, v), options)
            images[i] = image.handle
        end
        self.handle = Scheduler.awaitCall(engine.CreateCubeTexture, images, table.unpack(flags))
    else
        error('Unknown type: '..options.target)
    end
end

function Texture:_loadImage( fileName, options )
    local image = Image:load(fileName)
    if options.multiplyRgbByAlpha then
        image:multiplyRgbByAlpha()
    end

    local quality = Config.get('opengl.texture-quality', 1)
    assert(math.isInteger(quality) and quality >= 1)
    if quality > 1 then
        local scale = 1.0 / quality
        image = image:createResizedCopy(image.width  * scale,
                                        image.height * scale)
    end

    return image
end

function Texture:destroy()
    Scheduler.blindCall(engine.DestroyTexture, self.handle)
    self.handle = nil
end


return Texture
