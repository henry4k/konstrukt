#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'apoapsis.core.test.common'

local Mock = require 'test.mock.Mock'


describe('A mesh')
    :setup(function()
        NATIVE = {
            CreateMesh = Mock(),
            DestroyMesh = Mock()
        }

        Json = {
            decodeFromFile = Mock()
        }

        Scene = {
            createMeshBufferByPath = Mock()
        }

        FakeRequire:whitelist('apoapsis.core.Mesh')
        FakeRequire:whitelist('apoapsis.core.middleclass')
        FakeRequire:fakeModule('apoapsis.core.Resource', {})
        FakeRequire:fakeModule('apoapsis.core.Json', Json)
        FakeRequire:fakeModule('apoapsis.core.Scene', Scene)
        FakeRequire:install()

        Mesh = require 'apoapsis.core.Mesh'
    end)

    :beforeEach(function()
        NATIVE.CreateMesh:reset()
        NATIVE.DestroyMesh:reset()
        Json.decodeFromFile:reset()
        Scene.createMeshBufferByPath:reset()
    end)

    :it('can be created and destroyed.', function()
        local meshBuffer = { handle='mesh buffer handle' }

        Json.decodeFromFile:canBeCalled{with={'scene file name'},
                                        thenReturn={'scene'}}
        Scene.createMeshBufferByPath:canBeCalled{with={'scene', 'object name'},
                                                 thenReturn={meshBuffer}}
        NATIVE.CreateMesh:canBeCalled{with={'mesh buffer handle'},
                                      thenReturn={'mesh handle'}}
        NATIVE.DestroyMesh:canBeCalled{with={'mesh handle'}}

        local mesh = Mesh:load('scene file name', 'object name')

        assert(mesh.handle == 'mesh handle')
        Json.decodeFromFile:assertCallCount(1)
        Scene.createMeshBufferByPath:assertCallCount(1)
        NATIVE.CreateMesh:assertCallCount(1)

        mesh:destroy()

        assert(mesh.handle == nil)
        NATIVE.DestroyMesh:assertCallCount(1)
    end)


bdd.runTests()
