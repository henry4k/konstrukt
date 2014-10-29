#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('A 4x4 matrix')
    :setup(function()
        ENGINE = {
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
            ENGINE.CreateMatrix4:reset()
            ENGINE.CopyMatrix4:reset()
            ENGINE.Matrix4Op:reset()
            ENGINE.TranslateMatrix4:reset()
            ENGINE.ScaleMatrix4:reset()
            ENGINE.RotateMatrix4:reset()
            ENGINE.Matrix4TransformVector:reset()
            ENGINE.MakeRotationMatrix:reset()
        end

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/Vector')
        FakeRequire:whitelist('core/Matrix4')
        FakeRequire:install()

        Vec  = require 'core/Vector'
        Mat4 = require 'core/Matrix4'
    end)

    :beforeEach(function()
        ResetMocks()
    end)

    :it('can be created.', function()
        ENGINE.CreateMatrix4:canBeCalled{thenReturn={'the handle'}}

        local m = Mat4()
        assert(m.handle == 'the handle')
        ENGINE.CreateMatrix4:assertCallCount(1)
    end)

    :it('can be copied.', function()
        ENGINE.CreateMatrix4:canBeCalled{thenReturn={'handle a'}}
        ENGINE.CopyMatrix4:canBeCalled{with={'handle a'}, thenReturn={'handle b'}}

        local a = Mat4()
        assert(a.handle == 'handle a')

        local b = a:copy()
        assert(b.handle == 'handle b')

        ENGINE.CreateMatrix4:assertCallCount(1)
        ENGINE.CopyMatrix4:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()
        ENGINE.CreateMatrix4:canBeCalled{thenReturn={'handle a'}}
        ENGINE.CreateMatrix4:canBeCalled{thenReturn={'handle b'}}
        a = Mat4()
        b = Mat4()
    end)

    :it('can perform arithmetic operations on other matrices.', function()
        ENGINE.Matrix4Op:canBeCalled{with={'handle a', 'handle b', '+'}, thenReturn={'handle +'}}
        ENGINE.Matrix4Op:canBeCalled{with={'handle a', 'handle b', '-'}, thenReturn={'handle -'}}
        ENGINE.Matrix4Op:canBeCalled{with={'handle a', 'handle b', '*'}, thenReturn={'handle *'}}
        ENGINE.Matrix4Op:canBeCalled{with={'handle a', 'handle b', '/'}, thenReturn={'handle /'}}

        assert((a+b).handle == 'handle +')
        assert((a-b).handle == 'handle -')
        assert((a*b).handle == 'handle *')
        assert((a/b).handle == 'handle /')

        ENGINE.Matrix4Op:assertCallCount(4)
    end)

    :beforeEach(function()
        ResetMocks()
        ENGINE.CreateMatrix4:canBeCalled{thenReturn={'handle a'}}
        a = Mat4()
    end)

    :it('can be translated by a vector.', function()
        ENGINE.TranslateMatrix4:canBeCalled{with={'handle a', 10, 20, 30},
                                            thenReturn={'handle b'}}
        local b = a:translate(Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        ENGINE.TranslateMatrix4:assertCallCount(1)
    end)

    :it('can be scaled by a vector.', function()
        ENGINE.ScaleMatrix4:canBeCalled{with={'handle a', 10, 20, 30},
                                        thenReturn={'handle b'}}
        local b = a:scale(Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        ENGINE.ScaleMatrix4:assertCallCount(1)
    end)

    :it('can be rotated.', function()
        ENGINE.RotateMatrix4:canBeCalled{with={'handle a', 45, 10, 20, 30},
                                         thenReturn={'handle b'}}
        local b = a:rotate(45, Vec(10, 20, 30))
        assert(b.handle == 'handle b')
        ENGINE.RotateMatrix4:assertCallCount(1)
    end)

    :it('can transform a vector.', function()
        ENGINE.Matrix4TransformVector:canBeCalled{with={'handle a', 1, 2, 3, 4},
                                                  thenReturn={10, 20, 30, 40}}
        local v = a:transform(Vec(1, 2, 3, 4))
        assert(v == Vec(10, 20, 30, 40))
        ENGINE.Matrix4TransformVector:assertCallCount(1)
    end)

    :it('can be trimmed to a rotation matrix.', function()
        ENGINE.MakeRotationMatrix:canBeCalled{with={'handle a'}, thenReturn={'handle b'}}
        local b = a:toRotationMatrix()
        assert(b.handle == 'handle b')
        ENGINE.MakeRotationMatrix:assertCallCount(1)
    end)


bdd.runTests()
