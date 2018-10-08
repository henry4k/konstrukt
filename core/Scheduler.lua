local engine = require'rawengine'
local class = require'middleclass'
local Coroutine = require'core/Coroutine':subclass'core/SchedulerCoroutine'


local CallQueue = class'core/CallQueue'

function CallQueue:initialize()
    self._queue = {}
end

function CallQueue:enqueue(fn, ...)
    local call = {fn, ...}
    table.insert(self._queue, call)
end

function CallQueue:execute()
    for _, call in ipairs(self._queue) do
        local fn = call[1]
        fn(table.unpack(call, 2))
        -- error handling?
        -- return values?
    end
    self._queue = {}
end


local serialQueue = CallQueue()
local parallelQueue = CallQueue()
local inSerialPhase = true

local function awaitQueueExecution(queue)
    local coro = assert(Coroutine:running())
    queue:enqueue(coro.call, coro)
    Coroutine:yield()
end

local function awaitSerialPhase()
    if not inSerialPhase then
        return awaitQueueExecution(serialQueue)
    end
end

local function awaitParallelPhase()
    if inSerialPhase then
        return awaitQueueExecution(parallelQueue)
    end
end

local function awaitCall(fn, ...)
    if inSerialPhase then
        return fn(...)
    else
        awaitSerialPhase()
        local results = {pcall(fn, ...)}
        awaitParallelPhase()
        local success = results[1]
        if success then
            return table.unpack(results, 2)
        else
            local err = results[2]
            error(err)
        end
    end
end

local function blindCall(fn, ...)
    if inSerialPhase then
        return fn(...)
    else
        return serialQueue:enqueue(fn, ...)
    end
end

engine.SetCallback('serial', function()
    inSerialPhase = true
    serialQueue:execute()
end)

engine.SetCallback('parallel', function(events)
    inSerialPhase = false
    parallelQueue:execute()
end)

local function createUserCoro(fn)
    return Coroutine(fn)
end

local function run(coro)
    assert(coro:isInstanceOf(Coroutine))
    parallelQueue:enqueue(coro.call, coro)
end

return {awaitCall = awaitCall,
        blindCall = blindCall,
        createScheduledCoroutine = Coroutine,
        _run = run}
