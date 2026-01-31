return {
	{
		"neovim/nvim-lspconfig",
		opts = {
			diagnostics = {
				virtual_text = {
					severity = { min = vim.diagnostic.severity.ERROR },
				},
			},
		},
	},
}
