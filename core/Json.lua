local fs = require 'core/FileSystem'


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
    local json = Json.encodeToString(value)
    fs.writeFile(fileName, json)
    return true
end

--- Try to read a json file.
-- @return
-- The decoded json object or nil it failed.
function Json.decodeFromFile( fileName )
    local fileData = fs.readFile(fileName)
    return Json.decodeFromString(fileData)
end

return Json
