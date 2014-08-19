#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Spy = require 'test/mock/Spy'
local class = require 'core/middleclass'
local Effect = require 'core/effect/Effect'
local EffectTarget = require 'core/effect/EffectTarget'


describe('An effect')
    :setup(function()
        TestTarget = class('TestTarget')
        TestTarget:include(EffectTarget)

        function TestTarget:initialize()
            self:initializeEffectTarget(self)
        end

        function TestTarget:destroy()
            self:destroyEffectTarget()
        end

        function TestTarget:tick( time )
            self:callEffects('onTick', time)
        end


        FakeEffect = class('FakeEffect', Effect)

        FakeEffect.initialize = Spy:new(function( self, target, ... )
            Effect.initialize(self, target)
        end)

        FakeEffect.destroy = Spy:new(function( self )
            Effect.destroy(self)
        end)

        FakeEffect.onTick = Spy:new(function( self, time ) end)
    end)

    :beforeEach(function()
        FakeEffect.initialize:reset()
        FakeEffect.destroy:reset()
        FakeEffect.onTick:reset()
    end)

    :it('can be added.', function()
        local target = TestTarget()
        local effect = target:addEffect(FakeEffect, 'green')

        FakeEffect.initialize:assertCallCount(1)
        FakeEffect.initialize:assertCalledWith(effect, target, 'green')
    end)

    :it('destructor is called on removal.', function()
        local target = TestTarget()
        local effect = target:addEffect(FakeEffect, 'green')
        target:removeEffect(effect)

        FakeEffect.destroy:assertCallCount(1)
        FakeEffect.destroy:assertCalledWith(effect)
    end)

    :it('destructor is called on removal.', function()
        local target = TestTarget()
        local effect = target:addEffect(FakeEffect, 'green')
        target:destroy()

        FakeEffect.destroy:assertCallCount(1)
        FakeEffect.destroy:assertCalledWith(effect)
    end)

    :it('destructor is called only once.', function()
        local target = TestTarget()
        local effect = target:addEffect(FakeEffect, 'green')
        target:destroy()
        target:destroy()
        target:removeEffect(effect)

        FakeEffect.destroy:assertCallCount(1)
        FakeEffect.destroy:assertCalledWith(effect)
    end)

    :it('reacts to events.', function()
        local target = TestTarget()
        local effect = target:addEffect(FakeEffect, 'green')
        target:tick(42)

        FakeEffect.onTick:assertCallCount(1)
        FakeEffect.onTick:assertCalledWith(effect, 42)
    end)


bdd.runTests()
