--- @module core.Json
--- Utility functions for working with JSON encoded files.


local cjson = require 'cjson'
local FS = require 'core/FileSystem'


local Json = {}

--- Encode a value as JSON string.
-- @return
-- The encoded JSON string or `nil` if it failed.
function Json.encodeToString( value )
    return cjson.encode(value)
end

--- Decode a JSON string to its value.
-- @return
-- The decoded JSON object or nil it failed.
function Json.decodeFromString( jsonString )
    return cjson.decode(jsonString)
end

--- Try to write a JSON file.
-- @return
-- Whether the operation succeeded.
function Json.encodeToFile( fileName, value )
    local json = Json.encodeToString(value)
    FS.writeFile(fileName, json)
    return true
end

--- Try to read a JSON file.
-- @return
-- The decoded JSON object or `nil` it failed.
function Json.decodeFromFile( fileName )
    local fileData = FS.readFile(fileName)
    return Json.decodeFromString(fileData)
end

return Json
