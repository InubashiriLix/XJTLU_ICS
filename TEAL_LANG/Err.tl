local interface ErrorType
    name: string | number
    ok: boolean
    msg: string
end

local record ForeignError is ErrorType
    name: string | number
    ok: boolean
    msg: string
end

function ForeignError.from(ok: boolean, code: string | nil | nil, msg: string | nil): ForeignError
    return setmetatable(
        {ok = ok, name = code, msg = msg or ""},
        {__index = ForeignError}
    )
end

function ForeignError:to_raw(): boolean, string | number | nil, string | nil
    if self.ok then
        return true, nil, nil
    else 
        return self.ok, self.name, self.msg
    end
end

-- use case
local record ServiceUnknownError is ErrorType
    name: string | number
    ok: boolean
    msg: string
end

-- to say the truth, regarding the parameter error in the development is not a 
-- runtime error but an unwrap error should raise as immediately as possible.
local record InvalidArgumentError is ErrorType
    name: string | number
    ok: boolean
    msg: string
    expect: string | number | nil
    got: string | number | nil
end

-- this fucntion will raise ServiceUnknownError | InvalidArguementError or just return a integer randomly. just for test
local function raiseServiceOrInvalidArgmentError():  integer | ServiceUnknownError | InvalidArgumentError
    local n = math.random(1, 3)
    if n == 1 then
        return ServiceUnknownError.from(false, "SERVICE_UNKNOWN", "Service failed")
    elseif n == 2 then
        return InvalidArgumentError.from(false, "INVALID_ARGUMENT", "Invalid argument")
    else
        return 42
    end

end
