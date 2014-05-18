#!/usr/bin/env busted

describe('Control module', function()
    it('can register key controls', function()

        Native = {}
        Native = mock()
        stub(Native, 'SetEventCallback')
        stub(Native, 'RegisterKeyControl')

        local control = require 'Control'
        assert.stub(Native.SetEventCallback).was.called_with('KeyControlAction', nil)


        control.RegisterKeyControl('MyKeyControl')

        assert.stub(Native.RegisterKeyControl).was.called_with('MyKeyControl')
    end)
end)
