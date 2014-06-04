local Json = {}

function Json.encodeToString( value )
    return cjson.encode(value)
end

function Json.decodeFromString( jsonString )
    return cjson.decode(jsonString)
end

function Json.encodeToFile( fileName, value )
    local json = Json.encodeToString(value)
    local file = io.open(fileName, 'w')
    file:write(json)
    file:close()
end

function Json.decodeFromFile( fileName )
    local file = io.open(fileName, 'r')
    local json = file:read('*a')
    file:close()
    return Json.decodeFromString(json)
end

return Json