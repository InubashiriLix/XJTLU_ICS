local search_table = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
-- O(n)
local target_num = 8
--- if the target num is at the nst place, then O(n)
--- else no
--- @param target_num number the target number
local function linear_search(target_num)
	for _, value in ipairs(search_table) do
		if target_num == value then
			print("hited")
			return nil
		end
	end
	print("not found")
end

linear_search(target_num)
