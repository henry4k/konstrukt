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


local syncQueue = CallQueue()
local workQueue = CallQueue()
local syncing = true

local function awaitQueueExecution(queue)
    local coro = assert(Coroutine:running())
    queue:enqueue(coro.call, coro)
    Coroutine:yield()
end

local function awaitSync()
    if not syncing then
        return awaitQueueExecution(syncQueue)
    end
end

local function awaitWork()
    if syncing then
        return awaitQueueExecution(workQueue)
    end
end

local function awaitCall(fn, ...)
    if syncing then
        return fn(...)
    else
        awaitSync()
        local results = {pcall(fn, ...)}
        awaitWork()
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
    if syncing then
        return fn(...)
    else
        return syncQueue:enqueue(fn, ...)
    end
end

engine.SetEventCallback('Sync', function()
    syncing = true
    syncQueue:execute()
end)

engine.SetEventCallback('Work', function()
    syncing = false
    workQueue:execute()
end)

local function createUserCoro(fn)
    return Coroutine(fn)
end

local function run(coro)
    assert(coro:isInstanceOf(Coroutine))
    workQueue:enqueue(coro.call, coro)
end

return {awaitCall = awaitCall,
        blindCall = blindCall,
        createScheduledCoroutine = Coroutine,
        _run = run}
