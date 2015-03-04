#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

local Mock = require 'test.mock.Mock'


describe('The configuration')
    :setup(function()
        ENGINE = {
            GetConfigValue = Mock()
        }

        Config = require 'core/Config'
    end)

    :beforeEach(function()
        ENGINE.GetConfigValue:reset()
    end)

    :it('can be queried.', function()
        ENGINE.GetConfigValue:canBeCalled{with={'foo.bar', 'default value'}, thenReturn={'config value'}}
        local value = Config.get('foo.bar', 'default value')
        assert(value == 'config value')
        ENGINE.GetConfigValue:assertCallCount(1)
    end)


bdd.runTests()
