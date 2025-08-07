local test_table = { 3, 5, 5, 12, 1 }

local function sorted(tbl)
	local temp = tbl[1]
	for _, value in ipairs(tbl) do
		if temp > value then
			return false
		end
	end
	return true
end

local function selection_sort(tbl)
	local rtn_tbl = {}
	while #tbl ~= 0 do
		local min = tbl[1]
		local index = 1
		for idx, value in ipairs(tbl) do
			-- find the min
			if min <= value then
				min = value
				index = idx
			end
		end
		-- pop the min
		table.remove(tbl, index)
		-- add the min to the return table head
		table.insert(rtn_tbl, 1, min)
	end
	return rtn_tbl
end

local function bubble_sort(tbl)
	while not sorted(tbl) do
		for i = 1, #tbl - 1 do
			if tbl[i] > tbl[i + 1] then
				local temp = tbl[i + 1]
				tbl[i + 1] = tbl[i]
				tbl[i] = temp
			end
		end
	end
	return tbl
end

-- local rtn_shit = selection_sort(test_table)
-- local rtn_shit = bubble_sort(test_table)
for _, value in ipairs(rtn_shit) do
	print(value)
end
