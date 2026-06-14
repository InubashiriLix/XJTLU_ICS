local function mystery(n)
	local r = 0
	for i = 1, n - 1 do
		for j = i + 1, n do
			for k = 1, j do
				print("foobar")
			end
		end
	end
	return r
end
