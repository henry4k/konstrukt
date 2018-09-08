--- @classmod core.graphics.RenderTarget
--- (aka framebuffer)


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Camera = require 'core/graphics/Camera'
local ShaderProgramSet = require 'core/graphics/ShaderProgramSet'


local RenderTarget = class('core/graphics/RenderTarget')

--- Abstract class.
--
-- Use @{core.graphics.DefaultRenderTarget} or
-- @{core.graphics.TextureRenderTarget} instead.
--
-- @see setShaderProgramSet
--
function RenderTarget:initialize( handle )
    assert(self.class ~= RenderTarget, 'RenderTarget is an abstract class and not meant to be instanciated directly.')
    assert(type(handle) == 'userdata',
           'Must be initialized with a render target handle.')
    self.handle = handle
    self.camerasByLayer = {}
    self.camerasByName = {}
    self.shaderProgramSet = nil
end

function RenderTarget:destroy()
    engine.DestroyRenderTarget(self.handle)
    self.handle = nil
end

--- A render target can use multiple cameras.
--
-- Geometry of cameras with a lower layer can't occlude geometry from higher
-- layers.  This can be used to separate HUD and background from the regular
-- scene.
--
function RenderTarget:setCamera( layer, name, camera )
    assert(math.isInteger(layer), 'Layer must be an integer.')
    assert(layer >= 0, 'Layer must be positive.')
    assert(type(name) == 'string', 'Name must be a string.')
    assert(Object.isInstanceOf(camera, Camera), 'Must be called with a camera.')
    engine.SetRenderTargetCamera(self.handle, camera.handle, layer)
    self.camerasByLayer[layer] = camera
    self.camerasByName[name] = camera
end

--- Return camera for given layer.
function RenderTarget:getCameraByLayer( layer )
    return self.camerasByLayer[layer]
end

--- Return camera that matches the name.
function RenderTarget:getCameraByName( name )
    return self.camerasByName[name]
end

--- Changes the used set of shader programs.
-- @param[type=core.graphics.ShaderProgramSet] shaderProgramSet
function RenderTarget:setShaderProgramSet( shaderProgramSet )
    assert(Object.isInstanceOf(shaderProgramSet, ShaderProgramSet),
           'Must be called with a shader program set.')
    engine.SetRenderTargetShaderProgramSet(self.handle, shaderProgramSet.handle)
    self.shaderProgramSet = shaderProgramSet
end

-- Retrieves the currently used set of shader programs.
-- @return[type=ShaderProgramSet]
function RenderTarget:getShaderProgramSet()
    return self.shaderProgramSet
end


return RenderTarget
