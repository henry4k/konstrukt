local taggedcoro = require'taggedcoro'


local USER_TAG = 'user'
local scheduled_calls = {}
local scheduled_call_results = {}

local function nearest_coro_tagged_with(tag)
    local coro = taggedcoro.running()
    while taggedcoro.tag(coro) ~= tag do
        print('tag', taggedcoro.tag(coro))
        coro = taggedcoro.parent(coro)
        if not coro then
            error('No parent coroutine is tagged with '..tag)
        end
    end
    return coro
end

local function schedule_call(fn, ...)
    local coro = nearest_coro_tagged_with(USER_TAG)
    table.insert(scheduled_calls, {coro=coro, fn=fn, args={...}})
    return taggedcoro.yield(USER_TAG)
end

local function dispatch_scheduled_calls()
    for _, call in ipairs(scheduled_calls) do
        print'dispatch call'
        local values = {call.fn(table.unpack(call.args))}
        table.insert(scheduled_call_results, {coro=call.coro, values=values})
    end
    scheduled_calls = {}
end

local function dispatch_scheduled_call_results()
    for _, result in ipairs(scheduled_call_results) do
        print'dispatch result'
        taggedcoro.call(result.coro, table.unpack(result.values))
    end
    scheduled_call_results = {}
end

local function run_scheduler(initial_thread)
    assert(taggedcoro.tag(initial_thread) == USER_TAG)
    taggedcoro.call(initial_thread)
    repeat
        dispatch_scheduled_calls()
        dispatch_scheduled_call_results()
    until #scheduled_calls == 0
end

local function create_user_coro(fn)
    return taggedcoro.create(USER_TAG, fn)
end

return {run = run_scheduler,
        call = schedule_call,
        create_coroutine = create_user_coro,
        dispatch_calls = dispatch_scheduled_calls,
        dispatch_results = dispatch_scheduled_call_results}
