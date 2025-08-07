local test_string = "afdsukhjasef"
local pattern = "jase"

local function simple_shit()
	local flag = false
	for i = 1, #test_string - #pattern + 1 do
		for j = 1, #pattern do
			if not pattern[j] == test_string[i + j] then
				flag = false
				break
			end
		end
	end
end
