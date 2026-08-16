local function hell()
   return "shit"
end

local shit = hell().match("s", "", 1)

local nonsence = { "stirb", "fur", "mich" }

for i, v in ipairs(nonsence) do
   print(v .. i)
end
