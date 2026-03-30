local uv = vim.uv or vim.loop
local build_and_run_keymap = "<F6>"
local run_cur_buf_keymap = "<F7>"

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

---Find the Maven project root from the current buffer or current working directory.
---@return integer|nil # Returns -1 on any failure; otherwise nil on success.
local function get_project_root()
	local start_path = vim.api.nvim_buf_get_name(0)
	if start_path == "" then
		start_path = uv.cwd()
	end

	local pom_path = vim.fs.find("pom.xml", {
		path = start_path,
		upward = true,
	})[1]

	if not pom_path then
		return nil
	end

	return vim.fs.dirname(pom_path)
end

---Resolve the Maven command for the current project.
---@param project_root string
---@return string
local function get_maven_cmd(project_root)
	if vim.fn.filereadable(project_root .. "/mvnw") == 1 then
		return "./mvnw"
	end

	return "mvn"
end

---Send a Maven command to an existing terminal buffer.
---@param maven_args string
---@param missing_pom_msg string
---@return integer|nil # Returns -1 on any failure; otherwise nil on success.
local function exec_maven_in_terminal(maven_args, missing_pom_msg)
	local project_root = get_project_root()
	if not project_root then
		vim.notify(missing_pom_msg, vim.log.levels.WARN)
		return -1
	end

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

	local command =
		string.format("cd %s && %s %s\n", vim.fn.shellescape(project_root), get_maven_cmd(project_root), maven_args)

	-- send the command to terminal
	vim.api.nvim_chan_send(job_id, command)
end

---Run Maven clean install in an existing terminal buffer.
---@return integer|nil # Returns -1 on any failure; otherwise nil on success.
local function exec_maven_clean_install()
	return exec_maven_in_terminal("clean install", "No pom.xml found for this Maven project.")
end

---Run Maven clean install, then start Spring Boot only if build succeeds.
---@return integer|nil # Returns -1 on any failure; otherwise nil on success.
local function exec_maven_clean_install_and_run()
	return exec_maven_in_terminal(
		"clean install && ./mvnw spring-boot:run",
		"No pom.xml found for this Spring Boot project."
	)
end

-- ============ setup keymap ============
vim.keymap.set(
	"n",
	build_and_run_keymap,
	exec_maven_clean_install_and_run,
	{ noremap = true, silent = true, desc = "Run Maven clean install and start Spring Boot in terminal" }
)

vim.keymap.set(
	"n",
	run_cur_buf_keymap,
	exec_maven_clean_install,
	{ noremap = true, silent = true, desc = "Run Maven clean install in terminal" }
)

-- return an empty table
return {}
