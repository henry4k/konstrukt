-- Setup error handling

NATIVE.SetErrorFunction(function( message )
    return debug.traceback(message)
end)
