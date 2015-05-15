#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'
local Spy = require 'test.mock.Spy'
local VoxelAccessor = require 'core/world/VoxelAccessor'


describe('A voxel accessor')
    :setup(function()
        voxel = {
            read = Spy(function() end),
            write = Spy(function() end)
        }
    end)

    :beforeEach(function()
        voxel.read:reset()
        voxel.write:reset()
    end)

    :it('has masks, which can be defined using :setMask()', function()
        local accessor = VoxelAccessor()
        accessor:setMask('foo', 1, 4)
        accessor:setMask('bar', 5, 4)

        accessor:read(voxel, 'foo')
        accessor:read(voxel, 'bar')
        voxel.read:assertCallMatches{arguments={voxel, 1, 4}, atIndex=1}
        voxel.read:assertCallMatches{arguments={voxel, 5, 4}, atIndex=2}
    end)

    :it('has masks, which can be defined using :addMask()', function()
        local accessor = VoxelAccessor()
        accessor:addMask('foo', 4)
        accessor:addMask('bar', 4)

        accessor:read(voxel, 'foo')
        accessor:read(voxel, 'bar')
        voxel.read:assertCallMatches{arguments={voxel, 1, 4}, atIndex=1}
        voxel.read:assertCallMatches{arguments={voxel, 5, 4}, atIndex=2}
    end)


bdd.runTests()
