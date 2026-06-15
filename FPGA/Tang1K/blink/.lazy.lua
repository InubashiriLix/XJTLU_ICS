local project_root = vim.fs.root(vim.fn.getcwd(), { ".lazy.lua" }) or vim.fn.getcwd()
local snippet_path = project_root .. "/.nvim/snippets"
local dict_path = project_root .. "/.nvim/dict/verilog.txt"

return {
	{
		"garymjr/nvim-snippets",
		optional = true,
		enabled = false,
	},

	{
		"L3MON4D3/LuaSnip",
		event = "InsertEnter",
		config = function()
			local ls = require("luasnip")

			ls.setup({
				history = true,
				updateevents = "TextChanged,TextChangedI",
				delete_check_events = "TextChanged",
			})

			require("luasnip.loaders.from_vscode").lazy_load({
				paths = {
					snippet_path,
				},
			})
		end,
	},

	{
		"saghen/blink.cmp",
		optional = true,
		dependencies = {
			{
				"Kaiser-Yang/blink-cmp-dictionary",
				dependencies = { "nvim-lua/plenary.nvim" },
			},
		},
		opts = function(_, opts)
			opts.snippets = opts.snippets or {}
			opts.snippets.preset = "luasnip"

			opts.sources = opts.sources or {}
			opts.sources.providers = opts.sources.providers or {}
			opts.sources.per_filetype = opts.sources.per_filetype or {}

			opts.sources.providers.verilog_keywords = {
				module = "blink-cmp-dictionary",
				name = "Verilog",
				min_keyword_length = 1,
				max_items = 20,
				score_offset = 80,
				opts = {
					dictionary_files = {
						dict_path,
					},
				},
			}

			opts.sources.per_filetype.verilog = {
				"lsp",
				"snippets",
				"verilog_keywords",
				"buffer",
				"path",
			}

			opts.sources.per_filetype.systemverilog = {
				"lsp",
				"snippets",
				"verilog_keywords",
				"buffer",
				"path",
			}
		end,
	},
}
