#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('The configuration')
    :setup(function()
        NATIVE = {
            GetConfigValue = Mock()
        }

        Config = require 'core/Config'
    end)

    :beforeEach(function()
        NATIVE.GetConfigValue:reset()
    end)

    :it('can be queried.', function()
        NATIVE.GetConfigValue:whenCalledWith{'foo.bar', 'default value', returns={'config value'}}
        local value = Config.get('foo.bar', 'default value')
        assert(value == 'config value')
        NATIVE.GetConfigValue:assertCallCount(1)
    end)


bdd.runTests()
