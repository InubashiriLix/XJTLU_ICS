SHELL := /usr/bin/env bash
.SHELLFLAGS := -eu -o pipefail -c
.RECIPEPREFIX := >
.NOTPARALLEL:

# =========================
# Project config
# =========================

TOP := top
BOARD := tangnano9k

FAMILY := GW1N-9C
DEVICE := GW1NR-LV9QN88PC6/I5

OSS_CAD_SUITE ?= /opt/oss-cad-suite

SRC := $(shell find src -type f \( -name '*.v' -o -name '*.sv' \) 2>/dev/null | sort)
CST := constraints/tangnano9k.cst

BUILD := build
JSON := $(BUILD)/$(TOP).json
PNR_JSON := $(BUILD)/$(TOP)_pnr.json
BITSTREAM := $(BUILD)/$(TOP).fs

# Only affects commands launched by make.
# It does NOT modify your global fish/zsh/bash environment.
ifneq ($(wildcard $(OSS_CAD_SUITE)/bin),)
export PATH := $(OSS_CAD_SUITE)/bin:$(PATH)
endif

# =========================
# Pretty output
# =========================

RESET := \033[0m
BOLD := \033[1m
BLUE := \033[1;34m
GREEN := \033[1;32m
YELLOW := \033[1;33m
RED := \033[1;31m

.PHONY: all help init check print-env format lint synth pnr pack sram flash clean distclean gitignore editorconfig

all: init check $(BITSTREAM)
> @printf "$(GREEN)[DONE]$(RESET) Build finished: $(BITSTREAM)\n"

# =========================
# Help
# =========================

help:
> @printf "\n$(BOLD)Tang Nano 9K FPGA Project$(RESET)\n"
> @printf "================================\n\n"
> @printf "$(BOLD)推荐流程 / Recommended flow:$(RESET)\n"
> @printf "  1. make check      检查工具链 / Check toolchain\n"
> @printf "  2. make format     格式化 Verilog，4 spaces / Format RTL with 4 spaces\n"
> @printf "  3. make lint       Verilog lint / Run Verible lint\n"
> @printf "  4. make            综合 + 布局布线 + 打包 / Synthesis + PnR + pack\n"
> @printf "  5. make sram       下载到 SRAM，断电丢失 / Program SRAM, volatile\n"
> @printf "  6. make flash      烧录到 Flash，断电保留 / Program Flash, persistent\n\n"
> @printf "$(BOLD)Targets:$(RESET)\n"
> @printf "  make help          显示此帮助 / Show this help\n"
> @printf "  make init          生成 .gitignore 和 .editorconfig / Generate project helper files\n"
> @printf "  make check         检查 Arch/FPGA 环境 / Check required tools\n"
> @printf "  make print-env     打印当前 Makefile 环境 / Print build environment\n"
> @printf "  make format        用 4 spaces 格式化 RTL / Format RTL with 4 spaces\n"
> @printf "  make lint          运行 verible-verilog-lint / Run linter\n"
> @printf "  make               生成 $(BITSTREAM)\n"
> @printf "  make sram          下载 bitstream 到 SRAM / Load to SRAM\n"
> @printf "  make flash         烧录 bitstream 到 Flash / Flash permanently\n"
> @printf "  make clean         删除 build/ / Remove build artifacts\n"
> @printf "  make distclean     删除 build/ 和临时波形 / Remove generated artifacts\n"
> @printf "  make gitignore     重新生成 .gitignore / Regenerate .gitignore\n"
> @printf "  make editorconfig  重新生成 .editorconfig / Regenerate .editorconfig\n\n"

# =========================
# Init files
# =========================

init: .gitignore .editorconfig
> @printf "$(GREEN)[DONE]$(RESET) Project helper files are ready\n"

.gitignore:
> @printf "$(BLUE)[INFO]$(RESET) Generating .gitignore\n"; \
> { \
>     printf '%s\n' '# Build artifacts'; \
>     printf '%s\n' 'build/'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '# Simulation artifacts'; \
>     printf '%s\n' 'sim/'; \
>     printf '%s\n' '*.vcd'; \
>     printf '%s\n' '*.fst'; \
>     printf '%s\n' '*.gtkw'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '# FPGA bitstreams / reports'; \
>     printf '%s\n' '*.fs'; \
>     printf '%s\n' '*.bit'; \
>     printf '%s\n' '*.json'; \
>     printf '%s\n' '*.rpt'; \
>     printf '%s\n' '*.log'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '# Editor / OS'; \
>     printf '%s\n' '.DS_Store'; \
>     printf '%s\n' '*.swp'; \
>     printf '%s\n' '*.swo'; \
> } > .gitignore; \
> printf "$(GREEN)[DONE]$(RESET) .gitignore generated\n"

.editorconfig:
> @printf "$(BLUE)[INFO]$(RESET) Generating .editorconfig\n"; \
> { \
>     printf '%s\n' 'root = true'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '[*]'; \
>     printf '%s\n' 'charset = utf-8'; \
>     printf '%s\n' 'end_of_line = lf'; \
>     printf '%s\n' 'insert_final_newline = true'; \
>     printf '%s\n' 'trim_trailing_whitespace = true'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '[*.{v,sv,vh,svh}]'; \
>     printf '%s\n' 'indent_style = space'; \
>     printf '%s\n' 'indent_size = 4'; \
>     printf '%s\n' 'tab_width = 4'; \
>     printf '%s\n' ''; \
>     printf '%s\n' '[Makefile]'; \
>     printf '%s\n' 'indent_style = tab'; \
> } > .editorconfig; \
> printf "$(GREEN)[DONE]$(RESET) .editorconfig generated\n"

gitignore:
> @rm -f .gitignore
> @$(MAKE) --no-print-directory .gitignore

editorconfig:
> @rm -f .editorconfig
> @$(MAKE) --no-print-directory .editorconfig

# =========================
# Environment check
# =========================

check:
> @missing=0; \
> printf "$(BLUE)[INFO]$(RESET) Checking project files\n"; \
> if [ -z "$(SRC)" ]; then \
>     printf "  $(RED)MISS$(RESET) RTL source files under src/\n"; \
>     missing=1; \
> else \
>     printf "  $(GREEN)OK$(RESET)   RTL sources: $(SRC)\n"; \
> fi; \
> if [ -f "$(CST)" ]; then \
>     printf "  $(GREEN)OK$(RESET)   Constraint file: $(CST)\n"; \
> else \
>     printf "  $(RED)MISS$(RESET) Constraint file: $(CST)\n"; \
>     missing=1; \
> fi; \
> printf "\n$(BLUE)[INFO]$(RESET) Checking required commands\n"; \
> for tool in yosys nextpnr-himbaechel gowin_pack openFPGALoader verible-verilog-lint verible-verilog-format; do \
>     if command -v "$$tool" >/dev/null 2>&1; then \
>         path=$$(command -v "$$tool"); \
>         version=$$($$tool --version 2>/dev/null | head -n 1 || true); \
>         printf "  $(GREEN)OK$(RESET)   %-24s %s\n" "$$tool" "$$path"; \
>         if [ -n "$$version" ]; then \
>             printf "       %-24s %s\n" "" "$$version"; \
>         fi; \
>     else \
>         printf "  $(RED)MISS$(RESET) %-24s\n" "$$tool"; \
>         missing=1; \
>     fi; \
> done; \
> printf "\n$(BLUE)[INFO]$(RESET) Checking optional commands\n"; \
> for tool in verilator; do \
>     if command -v "$$tool" >/dev/null 2>&1; then \
>         printf "  $(GREEN)OK$(RESET)   %-24s %s\n" "$$tool" "$$(command -v "$$tool")"; \
>     else \
>         printf "  $(YELLOW)WARN$(RESET) %-24s optional, useful for stronger lint/sim\n" "$$tool"; \
>     fi; \
> done; \
> if [ "$$missing" -ne 0 ]; then \
>     printf "\n$(RED)[FAIL]$(RESET) Environment check failed\n\n"; \
>     printf "$(BOLD)Arch install guide:$(RESET)\n"; \
>     printf "  基础工具 / basic tools:\n"; \
>     printf "    sudo pacman -S --needed git make direnv openfpgaloader verilator\n\n"; \
>     printf "  Verible, 用于 nvim LSP / lint / format:\n"; \
>     printf "    paru -S --needed verible-bin\n\n"; \
>     printf "  OSS CAD Suite, 提供 yosys / nextpnr-himbaechel / gowin_pack:\n"; \
>     printf "    paru -S --needed oss-cad-suite-build-bin\n\n"; \
>     printf "  如果你手动安装在 /opt/oss-cad-suite:\n"; \
>     printf "    本 Makefile 会在 make 子进程里临时使用 /opt/oss-cad-suite/bin\n"; \
>     printf "    不会污染你的全局 fish PATH。\n\n"; \
>     exit 1; \
> fi; \
> printf "\n$(GREEN)[DONE]$(RESET) Environment check passed\n"

print-env:
> @printf "$(BOLD)Makefile environment$(RESET)\n"
> @printf "  TOP           = $(TOP)\n"
> @printf "  BOARD         = $(BOARD)\n"
> @printf "  FAMILY        = $(FAMILY)\n"
> @printf "  DEVICE        = $(DEVICE)\n"
> @printf "  OSS_CAD_SUITE = $(OSS_CAD_SUITE)\n"
> @printf "  SRC           = $(SRC)\n"
> @printf "  CST           = $(CST)\n"
> @printf "  BUILD         = $(BUILD)\n"
> @printf "  BITSTREAM     = $(BITSTREAM)\n"
> @printf "\n"
> @printf "  yosys         = $$(command -v yosys 2>/dev/null || true)\n"
> @printf "  nextpnr       = $$(command -v nextpnr-himbaechel 2>/dev/null || true)\n"
> @printf "  gowin_pack    = $$(command -v gowin_pack 2>/dev/null || true)\n"
> @printf "  openFPGALoader= $$(command -v openFPGALoader 2>/dev/null || true)\n"

# =========================
# Quality
# =========================

format: init
> @trap 'printf "$(RED)[FAIL]$(RESET) Format failed\n"' ERR; \
> printf "$(BLUE)[INFO]$(RESET) Formatting RTL with 4 spaces\n"; \
> if [ -z "$(SRC)" ]; then \
>     printf "$(RED)[FAIL]$(RESET) No RTL source files found under src/\n"; \
>     exit 1; \
> fi; \
> command -v verible-verilog-format >/dev/null; \
> verible-verilog-format --inplace --indentation_spaces=4 $(SRC); \
> printf "$(GREEN)[DONE]$(RESET) Format finished\n"

lint: init
> @trap 'printf "$(RED)[FAIL]$(RESET) Lint failed\n"' ERR; \
> printf "$(BLUE)[INFO]$(RESET) Running Verible lint\n"; \
> if [ -z "$(SRC)" ]; then \
>     printf "$(RED)[FAIL]$(RESET) No RTL source files found under src/\n"; \
>     exit 1; \
> fi; \
> command -v verible-verilog-lint >/dev/null; \
> verible-verilog-lint --rules_config_search $(SRC); \
> printf "$(GREEN)[DONE]$(RESET) Verible lint passed\n"

# =========================
# Build flow
# =========================

synth: $(JSON)
> @printf "$(GREEN)[DONE]$(RESET) Synthesis target is ready: $(JSON)\n"

pnr: $(PNR_JSON)
> @printf "$(GREEN)[DONE]$(RESET) Place-and-route target is ready: $(PNR_JSON)\n"

pack: $(BITSTREAM)
> @printf "$(GREEN)[DONE]$(RESET) Bitstream target is ready: $(BITSTREAM)\n"

$(JSON): $(SRC)
> @trap 'printf "$(RED)[FAIL]$(RESET) Synthesis failed\n"' ERR; \
> mkdir -p "$(BUILD)"; \
> printf "$(BLUE)[INFO]$(RESET) Synthesis: Verilog -> JSON\n"; \
> yosys -p "read_verilog $(SRC); synth_gowin -top $(TOP) -json $(JSON)"; \
> printf "$(GREEN)[DONE]$(RESET) Synthesis finished: $(JSON)\n"

$(PNR_JSON): $(JSON) $(CST)
> @trap 'printf "$(RED)[FAIL]$(RESET) Place-and-route failed\n"' ERR; \
> mkdir -p "$(BUILD)"; \
> printf "$(BLUE)[INFO]$(RESET) Place-and-route: JSON -> routed JSON\n"; \
> nextpnr-himbaechel \
>     --json "$(JSON)" \
>     --write "$(PNR_JSON)" \
>     --device "$(DEVICE)" \
>     --vopt family="$(FAMILY)" \
>     --vopt cst="$(CST)"; \
> printf "$(GREEN)[DONE]$(RESET) Place-and-route finished: $(PNR_JSON)\n"

$(BITSTREAM): $(PNR_JSON)
> @trap 'printf "$(RED)[FAIL]$(RESET) Bitstream pack failed\n"' ERR; \
> mkdir -p "$(BUILD)"; \
> printf "$(BLUE)[INFO]$(RESET) Packing bitstream: routed JSON -> FS\n"; \
> gowin_pack -d "$(FAMILY)" -o "$(BITSTREAM)" "$(PNR_JSON)"; \
> printf "$(GREEN)[DONE]$(RESET) Bitstream generated: $(BITSTREAM)\n"

# =========================
# Program board
# =========================

sram: all
> @trap 'printf "$(RED)[FAIL]$(RESET) SRAM programming failed\n"' ERR; \
> printf "$(BLUE)[INFO]$(RESET) Programming SRAM with openFPGALoader\n"; \
> openFPGALoader -b "$(BOARD)" "$(BITSTREAM)"; \
> printf "$(GREEN)[DONE]$(RESET) SRAM programmed. This will be lost after power off.\n"

flash: all
> @trap 'printf "$(RED)[FAIL]$(RESET) Flash programming failed\n"' ERR; \
> printf "$(BLUE)[INFO]$(RESET) Programming Flash with openFPGALoader\n"; \
> openFPGALoader -b "$(BOARD)" -f "$(BITSTREAM)"; \
> printf "$(GREEN)[DONE]$(RESET) Flash programmed. This should persist after power cycle.\n"

# =========================
# Clean
# =========================

clean:
> @printf "$(BLUE)[INFO]$(RESET) Removing $(BUILD)/\n"; \
> rm -rf "$(BUILD)"; \
> printf "$(GREEN)[DONE]$(RESET) Clean finished\n"

distclean: clean
> @printf "$(BLUE)[INFO]$(RESET) Removing simulation/temp artifacts\n"; \
> rm -rf sim; \
> rm -f *.vcd *.fst *.gtkw *.log; \
> printf "$(GREEN)[DONE]$(RESET) Distclean finished\n"
