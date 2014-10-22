#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Spy = require 'test.mock.Spy'


describe('The resource manager')
    :setup(function()

        MeshDestructor = Spy(function( self ) end)

        MeshLoader = Spy(function( sceneFile, objectName )
            if sceneFile == 'AirLock.json' and objectName == 'AirLock.Door' then
                return {
                    name = objectName,
                    destroy = MeshDestructor
                }
            else
                return nil
            end
        end)

        FakeRequire:whitelist('core/ResourceManager')
        FakeRequire:fakeModule('core/Shutdown', {
            registerHandler = Spy(function( handler ) end)
        })
        FakeRequire:install()

        ResourceManager = require 'core/ResourceManager'
        ResourceManager.registerLoader('Mesh', MeshLoader)
    end)

    :beforeEach(function()
        ResourceManager.clear()
        ResourceManager.enableLoading(true)
        MeshDestructor:reset()
        MeshLoader:reset()
    end)

    :it('can\'t get or load resources of unknown types.', function()
        assert(ResourceManager.get('FooBar', 1, 2, 3) == nil)
        assert(pcall(ResourceManager.load, 'FooBar', 1, 2, 3) == false)
    end)

    :it('can\'t get or load non-existent resources.', function()
        assert(ResourceManager.get('Mesh', 'Jetpack.json', 'Jetpack.FuelTank') == nil)
        assert(ResourceManager.load('Mesh', 'Jetpack.json', 'Jetpack.FuelTank') == nil)
    end)

    :it('can\'t get existing resources, which are not loaded yet.', function()
        assert(ResourceManager.get('Mesh', 'AirLock.json', 'AirLock.Door') == nil)
    end)

    :it('can\'t load resources, when loading is disabled.', function()
        ResourceManager.enableLoading(false)
        assert(ResourceManager.load('Mesh', 'AirLock.json', 'AirLock.Door') == nil)
    end)

    :it('loads existing resources only once.', function()
        local mesh = ResourceManager.load('Mesh', 'AirLock.json', 'AirLock.Door')
        assert(mesh ~= nil)
        assert(mesh.name == 'AirLock.Door')

        local sameMesh = ResourceManager.load('Mesh', 'AirLock.json', 'AirLock.Door')
        assert(sameMesh == mesh)

        local sameMesh = ResourceManager.get('Mesh', 'AirLock.json', 'AirLock.Door')
        assert(sameMesh == mesh)

        MeshLoader:assertCallCount(1)
        MeshLoader:assertCallMatches{atIndex=1,
                                     arguments={'AirLock.json', 'AirLock.Door'}}
    end)

    :it('calls "destroy" in resources that have it.', function()
        local mesh = ResourceManager.load('Mesh', 'AirLock.json', 'AirLock.Door')
        ResourceManager.clear()
        ResourceManager.clear()
        MeshDestructor:assertCallCount(1)
        MeshDestructor:assertCallMatches{atIndex=1, arguments={mesh}}
    end)

    :it('doesn\'t attempt to call "destroy" in resources that don\'t have it.', function()
        local mesh = ResourceManager.load('Mesh', 'AirLock.json', 'AirLock.Door')
        mesh.destroy = nil
        ResourceManager.clear()
        ResourceManager.clear()
        MeshDestructor:assertCallCount(0)
    end)

    :it('can create unique identifiers for resources.', function()
        local a = ResourceManager._createResourceIdentifier(1,2,3)
        local b = ResourceManager._createResourceIdentifier(1,2,3)
        assert(a == b)

        local a = ResourceManager._createResourceIdentifier(1,2,3)
        local b = ResourceManager._createResourceIdentifier(1,2,4)
        assert(a ~= b)

        local a = ResourceManager._createResourceIdentifier('foo',true,{1,2,3})
        local b = ResourceManager._createResourceIdentifier('foo',true,{1,2,3})
        assert(a == b)

        local a = ResourceManager._createResourceIdentifier('foo',true,{1,2,3})
        local b = ResourceManager._createResourceIdentifier('foo',true,{1,2,4})
        assert(a ~= b)
    end)


bdd.runTests()
