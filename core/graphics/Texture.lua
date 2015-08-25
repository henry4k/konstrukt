--- @classmod core.graphics.Texture
--- Image, which can be directly used for rendering by the graphics card.
--
-- Includes @{core.Resource}.


local class    = require 'middleclass'
local Resource = require 'core/Resource'
local Load2dTexture   = ENGINE.Load2dTexture
local LoadCubeTexture = ENGINE.LoadCubeTexture
local DestroyTexture  = ENGINE.DestroyTexture


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
-- @param[type=table] flags
-- Flags for the texture.
--
-- Available are:
--
-- - `mipmap`:  Creates [mip maps](https://www.opengl.org/wiki/Texture#Mip_maps) for the texture.
-- - `filter`:  Render texture using at least a linear [filter](https://www.opengl.org/wiki/Sampler_Object#Filtering).
-- - `clamp`:  Limit texture coordinates to a range between 0 and 1.
--
function Texture.static:_load( target, fileName, flags )
    local texture = Texture(target, fileName, flags)
    return { value=texture, destructor=texture.destroy }
end

function Texture:initialize( target, fileName, flags )
    flags = flags or {}
    if target == '2d' then
        self.handle = Load2dTexture(fileName, table.unpack(flags))
    elseif target == 'cube' then
        self.handle = LoadCubeTexture(fileName, table.unpack(flags))
    else
        error('Unknown type: '..target)
    end
end

function Texture:destroy()
    DestroyTexture(self.handle)
    self.handle = nil
end


return Texture
