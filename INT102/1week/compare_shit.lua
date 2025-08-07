local ex = { 3, 2, 1 }

-- find the min value
local temp_max = ex[1]
for index, value in ipairs(ex) do
	temp_max = math.max(value, temp_max)
end

print(temp_max)
