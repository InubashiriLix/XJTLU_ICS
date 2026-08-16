local enum QueueError
    "Full"
end

local record Queue<T>
    items: {T}
    cursor: integer
    size: integer

    new: function(size: integer): Queue<T>
    push: function(self: Queue<T>, value: T): boolean, QueueError | nil, string | nil
    pop: function(self: Queue<T>, value: T): boolean, QueueError | nil, string | nil
end

function Queue.new<T>(size: integer): Queue<T>
    assert(size > 0, "queue size must be positive")

    local obj: Queue<T> = setmetatable(
        {
            items = {},
            cursor = 0,
            size = size,
        },
        {
            __index = Queue,
        }
    )

    return obj
end

function Queue:push<T>(value: T): boolean, QueueError | nil, string | nil
    if self.cursor >= self.size then
        return false, "Full", "queue is full"
    end

    self.cursor = self.cursor + 1
    self.items[self.cursor] = value

    return true, nil, nil
end

local queue: Queue<string> = Queue.new(10)
local ok, err, msg = queue:push("hello")
print(ok, err, msg)
