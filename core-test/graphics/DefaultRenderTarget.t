#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('The default render target')
    :setup(function()
        ENGINE = {
            GetDefaultRenderTarget = Mock(),
            DestroyRenderTarget = Mock()
        }

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/graphics/DefaultRenderTarget')
        FakeRequire:whitelist('core/graphics/RenderTarget')
        FakeRequire:install()

        DefaultRenderTarget = require 'core/DefaultRenderTarget'
    end)

    :beforeEach(function()
        ENGINE.GetDefaultRenderTarget:reset()
        ENGINE.DestroyRenderTarget:reset()
    end)

    :it('is a singleton.', function()
        ENGINE.GetDefaultRenderTarget:canBeCalled{thenReturn={'the handle'}}

        local a = DefaultRenderTarget:get()
        assert(a.handle == 'the handle')
        local b = DefaultRenderTarget:get()
        assert(a == b)

        ENGINE.GetDefaultRenderTarget:assertCallCount(1)
    end)

    :it('can\'t be destroyed.', function()
        ENGINE.GetDefaultRenderTarget:canBeCalled{thenReturn={'the handle'}}
        ENGINE.DestroyRenderTarget:canBeCalled{with={'the handle'}}

        local renderTarget = DefaultRenderTarget:get()
        renderTarget:destroy()

        ENGINE.DestroyRenderTarget:assertCallCount(0)
    end)


bdd.runTests()
