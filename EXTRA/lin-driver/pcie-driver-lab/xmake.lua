set_project("pcie-driver-lab")
set_version("0.2.0")
set_xmakever("3.0.0")

option("pci_vendor")
    set_default("0xffff")
    set_showmenu(true)
    set_description("练习设备的 PCI Vendor ID（十六进制）")
option_end()

option("pci_device")
    set_default("0xffff")
    set_showmenu(true)
    set_description("练习设备的 PCI Device ID（十六进制）")
option_end()

target("pcie_lab")
    set_kind("phony")
    set_values("pcie_lab.vendor", get_config("pci_vendor") or "0xffff")
    set_values("pcie_lab.device", get_config("pci_device") or "0xffff")
    on_build(function (target)
        import("lib.detect.find_tool")

        local make = assert(find_tool("make"), "找不到 make")
        local python = assert(find_tool("python3") or find_tool("python"),
            "找不到 Python，无法为 clangd 生成 compile_commands.json")
        local kernel_release = os.iorunv("uname", {"-r"}):trim()
        local kernel_build = "/lib/modules/" .. kernel_release .. "/build"
        local project_dir = os.projectdir()
        local build_dir = project_dir .. "/build"
        local compile_db_tool = kernel_build ..
            "/scripts/clang-tools/gen_compile_commands.py"

        assert(os.isfile(kernel_build .. "/Makefile"),
            "找不到当前内核的构建目录：" .. kernel_build)
        assert(os.isfile(compile_db_tool),
            "内核 headers 中缺少：" .. compile_db_tool)

        os.mkdir(build_dir)

        -- 直接在源码目录调用 Kbuild，使编译数据库指向正在编辑的源文件。
        os.vrunv(make.program, {
            "-C", kernel_build,
            "M=" .. project_dir,
            "PCI_VENDOR_ID=" .. target:values("pcie_lab.vendor"),
            "PCI_DEVICE_ID=" .. target:values("pcie_lab.device"),
            "modules"
        })

        -- Kbuild clean 会递归删除 *.ko，先用非 .ko 后缀暂存最终模块。
        os.cp(project_dir .. "/pcie_lab.ko",
              build_dir .. "/pcie_lab.module-bin")

        -- 从 Kbuild 生成的 .cmd 文件提取所有真实内核编译参数，供 clangd 使用。
        os.vrunv(python.program, {
            compile_db_tool,
            "-d", project_dir,
            "-o", build_dir .. "/compile_commands.json",
            "modules.order"
        }, {curdir = project_dir})

        -- 源码根目录只保留 clangd 数据库；其余 Kbuild 中间文件立即清理。
        os.vrunv(make.program, {
            "-C", kernel_build,
            "M=" .. project_dir,
            "clean"
        })
        os.cp(build_dir .. "/pcie_lab.module-bin",
              build_dir .. "/pcie_lab.ko")
        os.tryrm(build_dir .. "/pcie_lab.module-bin")
        os.cp(build_dir .. "/compile_commands.json",
              project_dir .. "/compile_commands.json")
        os.tryrm(build_dir .. "/compile_commands.json")
    end)
    on_clean(function ()
        local project_dir = os.projectdir()
        os.tryrm(project_dir .. "/build/kbuild")
        os.tryrm(project_dir .. "/build/pcie_lab.ko")
        os.tryrm(project_dir .. "/compile_commands.json")
    end)

task("docs")
    set_menu {
        usage = "xmake docs",
        description = "将 Typst 入门教程编译为 build/PCIe驱动入门.pdf"
    }
    on_run(function ()
        import("lib.detect.find_tool")
        local typst = assert(find_tool("typst"),
            "找不到 typst；安装后也可直接阅读 docs/PCIe驱动入门.typ")
        os.mkdir("build")
        os.vrunv(typst.program, {
            "compile",
            "--root", ".",
            "docs/PCIe驱动入门.typ",
            "build/PCIe驱动入门.pdf"
        })
    end)
