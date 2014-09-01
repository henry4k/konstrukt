-- Setup error handling

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)


-- Register resources:

require 'core/Texture':registerResource()
require 'core/AudioBuffer':registerResource()
require 'core/Mesh':registerResource()
require 'core/Shader':registerResource()
require 'core/ShaderProgram':registerResource()