local cjson = require 'cjson'


local Json = {}

function Json.encodeToString( value )
    return cjson.encode(value)
end

function Json.decodeFromString( jsonString )
    return cjson.decode(jsonString)
end

--- Try to write a json file.
-- @return
-- Whether the operation succeeded.
function Json.encodeToFile( fileName, value )
    local file = io.open(fileName, 'w')
    if file then
        local json = Json.encodeToString(value)
        file:write(json)
        file:close()
        return true
    else
        return false
    end
end

--- Try to read a json file.
-- @return
-- The decoded json object or nil it failed.
function Json.decodeFromFile( fileName )
    local file = io.open(fileName, 'r')
    if file then
        local json = file:read('*a')
        file:close()
        return Json.decodeFromString(json)
    else
        return nil
    end
end

return Json
