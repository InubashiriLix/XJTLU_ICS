local cwd = vim.loop.cwd()
local mvn_compile_and_run = "exec:java"
local run_cur_java_proj_keymap = "<F7>"

---Find a usable terminal buffer number.
---Scans all buffers and returns the first loaded terminal buffer found.
---If only unloaded terminal buffers exist, it loads the first candidate
---(without displaying a window) and returns it if loading succeeds.
---If no terminal buffer exists, it notifies the user and returns nil.
---@return number|nil # Terminal buffer number, or nil if none is available.
local function get_any_terminal_buf()
	local buf_list = vim.api.nvim_list_bufs() -- list all the bufs
	local candidate_unloaded = nil -- candidation for unloaded terminal buffer

	for _, buf in ipairs(buf_list) do
		if vim.bo[buf].buftype == "terminal" then
			if vim.api.nvim_buf_is_loaded(buf) then
				-- if found, return buf directly
				return buf
			else
				-- not loaded, cnadidate it first, load them at the end
				candidate_unloaded = candidate_unloaded or buf
			end
		end
	end

	-- load the terminal buffers
	if candidate_unloaded then
		-- laod it (Note: not display it as windows)
		pcall(vim.fn.bufload, candidate_unloaded)
		if vim.api.nvim_buf_is_loaded(candidate_unloaded) then
			return candidate_unloaded
		end
	end

	-- NOTE: I insist that you should open the terminal yourself.
	vim.notify("No terminal buffer found, open it yourself", vim.log.levels.WARN)

	-- return nil if nothinig found, just a flag
	return nil
end

local function find_parent_dir_with_file(start_path, target_file)
	target_file = target_file or ".lazy.lua"
	if not start_path or start_path == "" then
		start_path = vim.uv.cwd()
	end
	local p = vim.fs.normalize(vim.fn.fnamemodify(start_path, ":p"))
	local st = vim.uv.fs_stat(p)
	if st and st.type == "file" then
		p = vim.fs.dirname(p)
	end
	while p and p ~= "" do
		local candidate = vim.fs.joinpath(p, target_file)
		if vim.uv.fs_stat(candidate) then
			return p
		end
		local parent = vim.fs.dirname(p)
		if not parent or parent == p then
			break
		end
		p = parent
	end

	return nil
end

---@return integer|nil # Returns -1 on any failure; otherwise nil on success.
local function exec_cur_java_proj()
	-- confirm that current buffer py file
	if vim.api.nvim_get_option_value("filetype", { buf = 0 }) ~= "java" then
		vim.notify("Current Buffer is not java script file.", vim.log.levels.WARN, { title = "F7 mvn exec" })
		return -1
	end

	-- get the abs path of current buffer
	local buf_abs_path = vim.api.nvim_buf_get_name(0)
	if buf_abs_path == "" then
		vim.notify("Current Buffer is not java script file.", vim.log.levels.WARN)
		return -1
	end

	-- get the root of project (find .lazy.lua)
	local proj_root_path = find_parent_dir_with_file(buf_abs_path)

	-- get the terminal buffer number
	local term_bufnr = get_any_terminal_buf()
	if term_bufnr == nil then
		vim.notify("No terminal buffer found, open it yourself", vim.log.levels.WARN)
		return -1
	end

	-- get the terminal's job id, and check if nil / 0 (hangup etc)
	local job_id = vim.b[term_bufnr].terminal_job_id
	if job_id == nil or job_id == 0 then
		vim.notify("Terminal buffer job is not running.", vim.log.levels.WARN)
		return -1
	end

	local command = "cd " .. proj_root_path .. " && " .. "mvn exec:java" .. "\n"
	-- send the command to terminal
	vim.api.nvim_chan_send(job_id, command)
end

-- TODO: launch main entrances

-- ============ setup keymap ============
vim.keymap.set(
	"n",
	run_cur_java_proj_keymap,
	exec_cur_java_proj,
	{ noremap = true, silent = true, desc = "Run current buf's java project in terminal" }
)

-- return an empty table
return {}
