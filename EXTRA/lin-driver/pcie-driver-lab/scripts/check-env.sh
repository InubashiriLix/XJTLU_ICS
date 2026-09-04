#!/usr/bin/env bash
set -eu

kernel_release="$(uname -r)"
kernel_build="/lib/modules/${kernel_release}/build"

printf '当前内核: %s\n' "${kernel_release}"

if command -v xmake >/dev/null 2>&1; then
    printf 'xmake:   %s\n' "$(command -v xmake)"
else
    printf '错误: 找不到 xmake。\n' >&2
    exit 1
fi

if [ -f "${kernel_build}/Makefile" ]; then
    printf '内核构建目录: %s\n' "${kernel_build}"
else
    printf '错误: %s 不存在；请安装与当前内核完全匹配的 headers。\n' \
        "${kernel_build}" >&2
    exit 1
fi

if command -v lspci >/dev/null 2>&1; then
    printf 'pciutils: %s\n' "$(command -v lspci)"
else
    printf '提示: 未找到 lspci；安装 pciutils 后才能方便地查看 PCI 设备。\n'
fi

printf '环境检查通过。\n'

