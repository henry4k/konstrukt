---
-- @module core.Json


local FS = require 'core/FileSystem'
local _Encode = cjson.encode
local _Decode = cjson.decode


local Json = {}

function Json.encodeToString( value )
    return _Encode(value)
end

function Json.decodeFromString( jsonString )
    return _Decode(jsonString)
end

--- Try to write a json file.
-- @return
-- Whether the operation succeeded.
function Json.encodeToFile( fileName, value )
    local json = Json.encodeToString(value)
    FS.writeFile(fileName, json)
    return true
end

--- Try to read a json file.
-- @return
-- The decoded json object or nil it failed.
function Json.decodeFromFile( fileName )
    local fileData = FS.readFile(fileName)
    return Json.decodeFromString(fileData)
end

return Json
