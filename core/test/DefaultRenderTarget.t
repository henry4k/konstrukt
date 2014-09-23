#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('The default render target')
    :setup(function()
        NATIVE = {
            GetDefaultRenderTarget = Mock(),
            DestroyRenderTarget = Mock()
        }

        FakeRequire:whitelist('core/DefaultRenderTarget')
        FakeRequire:whitelist('core/RenderTarget')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:install()

        DefaultRenderTarget = require 'core/DefaultRenderTarget'
    end)

    :beforeEach(function()
        NATIVE.GetDefaultRenderTarget:reset()
        NATIVE.DestroyRenderTarget:reset()
    end)

    :it('is a singleton.', function()
        NATIVE.GetDefaultRenderTarget:canBeCalled{thenReturn={'the handle'}}

        local a = DefaultRenderTarget:get()
        assert(a.handle == 'the handle')
        local b = DefaultRenderTarget:get()
        assert(a == b)

        NATIVE.GetDefaultRenderTarget:assertCallCount(1)
    end)

    :it('can\'t be destroyed.', function()
        NATIVE.GetDefaultRenderTarget:canBeCalled{thenReturn={'the handle'}}
        NATIVE.DestroyRenderTarget:canBeCalled{with={'the handle'}}

        local renderTarget = DefaultRenderTarget:get()
        renderTarget:destroy()

        NATIVE.GetDefaultRenderTarget:assertCallCount(1)
        NATIVE.DestroyRenderTarget:assertCallCount(0)
    end)


bdd.runTests()
