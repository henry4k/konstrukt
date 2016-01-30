#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

local Mock = require 'test.mock.Mock'


describe('A mesh')
    :setup(function()
        ENGINE = {
            CreateMesh = Mock(),
            DestroyMesh = Mock()
        }

        Json = {
            decodeFromFile = Mock()
        }

        Scene = {
            createMeshBufferByPath = Mock()
        }

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/graphics/Mesh')
        FakeRequire:fakeModule('core/Resource', {})
        FakeRequire:fakeModule('core/Json', Json)
        FakeRequire:fakeModule('core/Scene', Scene)
        FakeRequire:install()

        Mesh = require 'core/graphics/Mesh'
    end)

    :beforeEach(function()
        ENGINE.CreateMesh:reset()
        ENGINE.DestroyMesh:reset()
        Json.decodeFromFile:reset()
        Scene.createMeshBufferByPath:reset()
    end)

    :it('can be created and destroyed.', function()
        local meshBuffer = { handle='mesh buffer handle' }

        Json.decodeFromFile:canBeCalled{with={'scene file name'},
                                        thenReturn={'scene'}}
        Scene.createMeshBufferByPath:canBeCalled{with={'scene', 'object name'},
                                                 thenReturn={meshBuffer}}
        ENGINE.CreateMesh:canBeCalled{with={'mesh buffer handle'},
                                      thenReturn={'mesh handle'}}
        ENGINE.DestroyMesh:canBeCalled{with={'mesh handle'}}

        local mesh = Mesh:load('scene file name', 'object name')

        assert(mesh.handle == 'mesh handle')
        Json.decodeFromFile:assertCallCount(1)
        Scene.createMeshBufferByPath:assertCallCount(1)
        ENGINE.CreateMesh:assertCallCount(1)

        mesh:destroy()

        assert(mesh.handle == nil)
        ENGINE.DestroyMesh:assertCallCount(1)
    end)


bdd.runTests()
