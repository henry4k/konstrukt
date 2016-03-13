--- @classmod core.graphics.Shader
--- Shaders are compiled shader sources, like objects are compiled `.c/.cpp` sources.
--
-- Shaders alone aren't of any use.  They first need to be linked to into a
-- @{core.graphics.ShaderProgram}, which can then be used for rendering by the
-- graphics driver.
--
-- This is just a brief summary, the complete documentation is available at
-- http://www.opengl.org/wiki/GLSL_Object
--
-- Includes @{core.Resource}.
--
-- @see core.graphics.ShaderProgram


local engine     = require 'engine'
local class      = require 'middleclass'
local Resource   = require 'core/Resource'
local FileSystem = require 'core/FileSystem'


local Shader = class('core/graphics/Shader')
Shader:include(Resource)

--- Loads a shader from file.
--
-- See `Shader:loadFromFile` for arguments and usage.
--
-- @function static:load( t )
--
function Shader.static:_load( t )
    local shader = Shader:loadFromFile(t)
    return { value=shader, destructor=shader.destroy }
end

Shader.static.extensionToType =
{
    vert = 'vertex',
    frag = 'fragment'
}

Shader.static.typeToExtension =
{
    vertex   = 'vert',
    fragment = 'frag'
}

Shader.static.defaultGroupShaderTypes = {'vertex', 'fragment'}

--- Like `load` but loads multiple files at once.
--
-- @param[type=table] t
--
function Shader.static:loadGroup( t )
    local fileName = t.fileName or t[1]
    local shaderTypes = t.shaderTypes or Shader.defaultGroupShaderTypes

    local args = table.copy(t)
    args.fileName = nil
    args[1] = nil
    args.shaderTypes = nil

    local shaders = {}
    for _, shaderType in ipairs(shaderTypes) do
        local extension = Shader.typeToExtension[shaderType]
        assert(extension)
        args.fileName = string.format('%s.%s', fileName, extension)
        args.shaderType = shaderType
        local shader = Shader:load(args)
        table.insert(shaders, shader)
    end

    return shaders
end

function Shader.static:_getShaderTypeFromFileName( fileName )
    local extension = fileName:match('%.(%w+)$')
    return Shader.extensionToType[extension]
end

function Shader.static:_generatePreamble( sourceName, version, definitions )
    local buffer = {}
    table.insert(buffer, string.format('#version %d', version))
    table.insert(buffer, '#line 0 preamble')
    for key, value in pairs(definitions) do
        if type(value) == 'boolean' then
            if value then
                table.insert(buffer, string.format('#define %s', key))
            end
        else
            local statement = string.format('#define %s %s', key, tostring(value))
            table.insert(buffer, statement)
        end
    end
    table.insert(buffer, string.format('#line 0 %s', sourceName))
    return table.concat(buffer, '\n')
end

function Shader.static:_insertPreamble( sourceName, source, definitions )
    local version = source:match('#version (%d+)')
    source = source:gsub('#version.-\n', '')
    local preamble = Shader:_generatePreamble(sourceName, version, definitions)
    return preamble..source
end

--- Loads a shader from file.
--
-- @param[type=table] t
-- `fileName`: Mandatory! May also be passed as as first positional argument.
-- `shaderType`: If not defined, it will be deduced from the files name.
-- `definitions`: Key/value table of preprocessor definitions, which shall be
-- available in the shader.
--
function Shader.static:loadFromFile( t )
    local fileName = assert(t.fileName or t[1], 'No file name given.')

    local shaderType = t.shaderType or Shader:_getShaderTypeFromFileName(fileName)
    if not shaderType then
        error(string.format('Shader type unknown.  It was neither explicitly defined nor can it be deduced from "%s"', fileName))
    end

    local definitions = t.definitions or {}

    local source = FileSystem.readFile(fileName)
    source = Shader:_insertPreamble(fileName, source, definitions)

    return Shader(shaderType, source)
end

function Shader:initialize( shaderType, source )
    self.handle = engine.CreateShader(shaderType, source)
end

function Shader:destroy()
    engine.DestroyShader(self.handle)
    self.handle = nil
end


return Shader
