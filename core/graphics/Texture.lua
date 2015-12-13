--- @classmod core.graphics.Texture
--- Image, which can be directly used for rendering by the graphics card.
--
-- Includes @{core.Resource}.


local class    = require 'middleclass'
local Resource = require 'core/Resource'
local Image    = require 'core/graphics/Image'
local Create2dTexture   = ENGINE.Create2dTexture
local CreateCubeTexture = ENGINE.CreateCubeTexture
local DestroyTexture    = ENGINE.DestroyTexture


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
function Texture.static:_load( options )
    local texture = Texture(options)
    return { value=texture, destructor=texture.destroy }
end

local cubeMapSides = { 'px', 'nx', 'py', 'ny', 'pz', 'nz' }

function Texture:initialize( options )
    local target   = options.target or '2d'
    local fileName = options.fileName
    local filter   = options.filter or true
    local wrapMode = options.wrapMode or 'repeat'

    local flags = {}
    table.insert(flags, 'mipmap') -- Always generate mip maps - for now.
    if filter then
        table.insert(flags, 'filter')
    end
    if wrapMode == 'clamp' then
        table.insert(flags, 'clamp')
    end

    if target == '2d' then
        local image = Image:load(fileName)
        image:multiplyRgbByAlpha()
        self.handle = Create2dTexture(image.handle, table.unpack(flags))
    elseif target == 'cube' then
        local images = {}
        for i,v in ipairs(cubeMapSides) do
            local image = Image:load(string.format(fileName, v))
            image:multiplyRgbByAlpha()
            images[i] = image.handle
        end
        self.handle = CreateCubeTexture(images, table.unpack(flags))
    else
        error('Unknown type: '..target)
    end
end

function Texture:destroy()
    DestroyTexture(self.handle)
    self.handle = nil
end


return Texture
