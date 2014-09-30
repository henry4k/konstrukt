local ReadFile  = ENGINE.ReadFile
local WriteFile = function() error('Function not implemented yet.') end


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
    WriteFile(fileName, json)
    return true
end

--- Try to read a json file.
-- @return
-- The decoded json object or nil it failed.
function Json.decodeFromFile( fileName )
    local fileData = ReadFile(fileName)
    return Json.decodeFromString(fileData)
end

return Json
