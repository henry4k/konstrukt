#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('A 4x4 matrix')
    :setup(function()
        NATIVE = {
            CreateMatrix4 = Mock(),
            CopyMatrix4 = Mock(),
            Matrix4Op = Mock(),
            TranslateMatrix4 = Mock(),
            ScaleMatrix4 = Mock(),
            RotateMatrix4 = Mock(),
            Matrix4TransformVector = Mock(),
            MakeRotationMatrix = Mock()
        }

        ResetMocks = function()
            NATIVE.CreateMatrix4:reset()
            NATIVE.CopyMatrix4:reset()
            NATIVE.Matrix4Op:reset()
            NATIVE.TranslateMatrix4:reset()
            NATIVE.ScaleMatrix4:reset()
            NATIVE.RotateMatrix4:reset()
            NATIVE.Matrix4TransformVector:reset()
            NATIVE.MakeRotationMatrix:reset()
        end

        FakeRequire:whitelist('core/Matrix4')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:whitelist('core/Vector')
        FakeRequire:install()

        Vec  = require 'core/Vector'
        Mat4 = require 'core/Matrix4'
    end)

    :beforeEach(function()
        ResetMocks()
    end)

    :it('can be created.', function()
        NATIVE.CreateMatrix4:whenCalledWith{returns={'the handle'}}

        local m = Mat4()
        assert(m.handle == 'the handle')
        NATIVE.CreateMatrix4:assertCallCount(1)
    end)

    :it('can be copied.', function()
        NATIVE.CreateMatrix4:whenCalledWith{returns={'handle a'}}
        NATIVE.CopyMatrix4:whenCalledWith{'handle a', returns={'handle b'}}

        local a = Mat4()
        assert(a.handle == 'handle a')

        local b = a:copy()
        assert(b.handle == 'handle b')

        NATIVE.CreateMatrix4:assertCallCount(1)
        NATIVE.CopyMatrix4:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()
        NATIVE.CreateMatrix4:whenCalledWith{returns={'handle a'}}
        NATIVE.CreateMatrix4:whenCalledWith{returns={'handle b'}}
        a = Mat4()
        b = Mat4()
    end)

    :it('can perform arithmetic operations on other matrices.', function()
        NATIVE.Matrix4Op:whenCalledWith{'handle a', 'handle b', '+', returns={'handle +'}}
        NATIVE.Matrix4Op:whenCalledWith{'handle a', 'handle b', '-', returns={'handle -'}}
        NATIVE.Matrix4Op:whenCalledWith{'handle a', 'handle b', '*', returns={'handle *'}}
        NATIVE.Matrix4Op:whenCalledWith{'handle a', 'handle b', '/', returns={'handle /'}}

        assert((a+b).handle == 'handle +')
        assert((a-b).handle == 'handle -')
        assert((a*b).handle == 'handle *')
        assert((a/b).handle == 'handle /')

        NATIVE.Matrix4Op:assertCallCount(4)
    end)

    :beforeEach(function()
        ResetMocks()
        NATIVE.CreateMatrix4:whenCalledWith{returns={'handle a'}}
        a = Mat4()
    end)

    :it('can be translated by a vector.', function()
        NATIVE.TranslateMatrix4:whenCalledWith{'handle a', 10, 20, 30, returns={'handle b'}}
        local b = a:translate(Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        NATIVE.TranslateMatrix4:assertCallCount(1)
    end)

    :it('can be scaled by a vector.', function()
        NATIVE.ScaleMatrix4:whenCalledWith{'handle a', 10, 20, 30, returns={'handle b'}}
        local b = a:scale(Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        NATIVE.ScaleMatrix4:assertCallCount(1)
    end)

    :it('can be rotated.', function()
        NATIVE.RotateMatrix4:whenCalledWith{'handle a', 45, 10, 20, 30, returns={'handle b'}}
        local b = a:rotate(45, Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        NATIVE.RotateMatrix4:assertCallCount(1)
    end)

    :it('can transform a vector.', function()
        NATIVE.Matrix4TransformVector:whenCalledWith{'handle a', 1, 2, 3, 4, returns={10, 20, 30, 40}}
        local v = a:transform(Vec(1, 2, 3, 4))
        assert(v == Vec(10, 20, 30, 40))
        NATIVE.Matrix4TransformVector:assertCallCount(1)
    end)

    :it('can be trimmed to a rotation matrix.', function()
        NATIVE.MakeRotationMatrix:whenCalledWith{'handle a', returns={'handle b'}}
        local b = a:toRotationMatrix()
        assert(b.handle == 'handle b')
        NATIVE.MakeRotationMatrix:assertCallCount(1)
    end)


bdd.runTests()
