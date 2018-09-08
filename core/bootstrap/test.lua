local engine = require'engine'
local scheduler = require'core/scheduler'

engine.SetEventCallback('Work', function()
    print'work'
    scheduler.dispatch_results()
end)

engine.SetEventCallback('Sync', function()
    print'sync'
    scheduler.dispatch_calls()
end)
