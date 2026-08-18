local function hell(): string
    return "shit"
end

local shit = hell().match("s", "", 1)

local nonsence: {string} = {"stirb", "fur", "mich"}

for i, v in ipairs(nonsence) do
    print(i .. " " .. v)
end
