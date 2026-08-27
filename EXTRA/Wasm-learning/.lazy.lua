local run_wat = function()
    -- get currrent buffer's absolute path
    local file_path = vim.fn.expand("%:p")
    --get the directory of the current buffer
    local dir = vim.fn.expand("%:p:h")
    -- get current buffer's filetype
    local filetype = vim.bo.filetype
    -- generate wasm file from wat file to the same directory with wat file
    local wasm_file = dir .. "/" .. vim.fn.expand("%:t:r") .. ".wasm"

    -- check if filetype is wat
    if filetype == "wat" then
        -- run wat2wasm command with the current buffer's path and wasm file path
        vim.fn.system("wat2wasm " .. file_path .. " -o " .. wasm_file)
    end

    -- then we have got the wasm file, we can run it with wasmtime
    local ok, result = pcall(vim.fn.system, "wasmtime run --invoke main " .. wasm_file)
    -- we should also print the run result or error using info
    if not ok then
        vim.notify("pcall failed with msg: " .. result, vim.log.levels.WARN, {})
    else
        -- else print the output
        vim.notify(result, vim.log.levels.INFO, {})
    end
end

vim.api.nvim_create_user_command("RunWat", run_wat, {})

return {
    {
        "neovim/nvim-lspconfig",
        opts = {
            diagnostics = {
                underline = false,
            },
            -- the server to install: wasm-language-tools
            servers = {
                wasm_language_tools = {
                    mason = true,
                    filetypes = {
                        "wat",
                        "wast",
                        "wasm",
                    },
                }

            },
        },
    }
}
