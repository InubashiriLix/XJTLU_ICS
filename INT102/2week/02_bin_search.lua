search_table = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }

local function is_sorted_small_2_big()
	local temp = search_table[1]
	for _, value in search_table do
		if value < temp then
			return false
		end
	end
	return true
end

local function slice(tbl, start_idx, end_idx)
	local result = {}
	table.move(tbl, start_idx, end_idx, 1, result)
	return result
end

---
--- binary search
--- when the search table is sorted previously
---
--- @param target_num number
--- @param search_table table (should be previously sorted)
---
local function binary_search(target_num, search_table, start_idx, end_idx)
	if target_num < search_table[1] or target_num > search_table[#search_table] then
		return false
	end
	if #search_table == 1 and target_num == search_table[1] then
		print("got")
		return
	elseif #search_table == 1 and target_num ~= search_table[1] then
		print("fuck")
		return
	elseif #search_table == 2 then
		if target_num == search_table[1] then
			print("hit")
			return
		elseif target_num == search_table[2] then
			print("hit")
			return
		else
			print("fuck")
			return
		end
	else
	end
	local mid_idx = math.floor(#search_table / 2)
	start_idx = target_num <= search_table[mid_idx] and 1 or mid_idx + 1
	end_idx = target_num <= search_table[mid_idx] and mid_idx or #search_table
	search_table = slice(search_table, start_idx, end_idx)
	for _, value in ipairs(search_table) do
		print(value)
	end
	binary_search(target_num, search_table, start_idx, end_idx)
end

binary_search(8, search_table, 1, #search_table)
