FakeRequire = {
    originalRequire = require,
    fakedModules = {},
    whitelisted = {}
}


function FakeRequire:fakeModule( name, table )
    self.fakedModules[name] = table
end

function FakeRequire:whitelist( prefix )
    table.insert(self.whitelisted, '^'..prefix)
end

function FakeRequire:isWhitelisted( name )
    for _, pattern in ipairs(self.whitelisted) do
        if string.find(name, pattern) then
            return true
        end
    end
    return false
end

function FakeRequire._require( moduleName )
    local module = FakeRequire.fakedModules[moduleName]
    if module then return module end

    if FakeRequire:isWhitelisted(moduleName) then
        return FakeRequire.originalRequire(moduleName)
    else
        error('Requiring non whitelisted module: '..moduleName, 2)
    end
end

function FakeRequire:install()
    require = self._require
end

function FakeRequire:uninstall()
    require = self.originalRequire
end


return FakeRequire
