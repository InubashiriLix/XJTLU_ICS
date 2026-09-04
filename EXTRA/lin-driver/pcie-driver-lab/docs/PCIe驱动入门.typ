#set document(
  title: "从空项目开始写 Linux PCIe 驱动",
  author: "PCIe Driver Lab",
)

#set page(
  paper: "a4",
  margin: (x: 2.2cm, y: 2cm),
  numbering: "1 / 1",
  header: context {
    if counter(page).get().first() > 1 {
      set text(size: 8.5pt, fill: luma(90))
      grid(
        columns: (1fr, auto),
        "Linux PCIe 驱动入门实验",
        emph("跟着文档亲手写代码"),
      )
      line(length: 100%, stroke: .4pt + luma(190))
    }
  },
)

#set text(
  font: ("Noto Serif CJK SC", "New Computer Modern"),
  lang: "zh",
  size: 10.5pt,
)
#set par(justify: true, leading: .72em)
#set heading(numbering: "1.1")
#set list(indent: 1.2em, body-indent: .55em)
#show raw: set text(
  font: ("DejaVu Sans Mono", "Noto Sans CJK SC"),
  size: 8.35pt,
)

#let accent = rgb("#245a9a")
#let safe = rgb("#eaf6ee")
#let caution = rgb("#fff4df")
#let info = rgb("#edf4fb")

#let callout(title, color, body) = block(
  width: 100%,
  fill: color,
  inset: 10pt,
  radius: 4pt,
  breakable: true,
)[
  *#title* #h(0.5em) #body
]

#align(center)[
  #v(1.35cm)
  #text(size: 24pt, weight: "bold", fill: accent)[
    从空项目开始写 Linux PCIe 驱动
  ]
  #v(10pt)
  #text(size: 15pt)[代码跟写版 · 只观察，不接管硬件]
  #v(24pt)
  #line(length: 72%, stroke: 1pt + accent)
  #v(24pt)
  #text(size: 11pt, fill: luma(65))[
    xmake · Kbuild · clangd · PCI 匹配 · probe · 安全实验流程
  ]
  #v(2cm)
]

#callout("学习方式", info)[
  `src/pcie_lab.c` 只包含能通过构建的最小模块元数据，不包含完成版驱动。
  从第 5 章开始，你会按照文档中的代码一步步修改它，并在每个检查点编译验证。
]

#v(8pt)

#callout("安全边界", caution)[
  PCI 练习阶段默认匹配保留值 `ffff:ffff`。教程代码只读取 Linux 已缓存的设备信息，
  `probe()` 最终返回 `-ENODEV`；它不启用设备、不映射 MMIO、不注册中断，也不启动
  DMA。首次加载自写模块仍建议使用带快照的虚拟机。
]

#v(1fr)
#align(center)[
  #text(size: 9pt, fill: luma(100))[
    对应项目：`pcie-driver-lab` #h(1em) · #h(1em) 教程版本：0.2.0
  ]
]

#pagebreak()

#outline(title: [目录], indent: auto)

#pagebreak()

= 你拿到的是一个空练习项目

目录结构如下：

```text
pcie-driver-lab/
├── .clangd
├── xmake.lua
├── Kbuild
├── src/
│   └── pcie_lab.c
├── scripts/
│   └── check-env.sh
└── docs/
    └── PCIe驱动入门.typ
```

此时 `src/pcie_lab.c` 只有下面这些内容：

```c
// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PCIe driver learner");
MODULE_DESCRIPTION("Empty Linux PCIe driver lab");
MODULE_VERSION("0.1.0");
```

它还没有入口函数、PCI ID 表或 `probe()`。保留 `MODULE_LICENSE()` 是因为现代内核的
模块检查阶段要求许可证元数据；这只是一个可编译的空起点，不是教程答案。

#callout("建议的练习节奏", safe)[
  每完成一章就先运行 `xmake`，修完当前错误后再继续。不要一次复制最终代码；亲手
  遇到并定位一次编译错误，比直接得到一个完整文件更有学习价值。
]

= 构建系统分别做什么

- `xmake.lua` 提供统一命令，保存练习用 Vendor/Device ID，并编译 Typst 文档。
- `Kbuild` 描述 Linux 内核模块由哪个源文件组成。
- Linux Kbuild 提供目标内核真正需要的头文件、宏、架构参数、符号版本和链接步骤。
- xmake 在构建结束后自动生成 `compile_commands.json`，让 clangd 使用同一套参数。

内核模块不能像用户态程序那样只执行 `gcc file.c`。诸如 `linux/init.h` 的头文件来自
当前内核的构建目录，而不是普通的 `/usr/include` 搜索路径。

= 先解决 clangd 的头文件问题

== 为什么会出现一长串错误

`'linux/init.h' file not found` 是第一个真正的问题。后面的 `dev_info` 未声明、
`struct pci_dev` 不完整等信息只是它引发的连锁错误。

clangd 不会自动猜测 Kbuild 的参数。它需要知道：

+ 当前内核的普通、架构相关和 generated include 目录；
+ `__KERNEL__`、`MODULE`、`KBUILD_MODNAME` 等宏；
+ 内核强制包含的配置与编译器类型头文件；
+ 内核采用的 C 标准与架构编译选项。

只手工添加一个 `/usr/src/linux/include` 并不能正确解决这些问题。

== 让 xmake 自动生成正确参数

在项目根目录运行：

```bash
./scripts/check-env.sh
xmake f -c
xmake
```

构建成功后会同时得到：

```text
build/pcie_lab.ko
compile_commands.json
```

其中 `compile_commands.json` 是根据 Kbuild 实际执行过的命令生成的，而且指向你正在
编辑的 `src/pcie_lab.c`。clangd 会从 `src/` 向父目录查找并自动发现它。

生成后重启一次语言服务器。Neovim 通常可执行：

```vim
:LspRestart
```

也可以直接检查 clangd 是否正确读取数据库：

```bash
clangd --check=src/pcie_lab.c --compile-commands-dir=.
```

本机的 clangd 22 在 `--check` 模式下可能最后报告 `tweak: SwapBinaryOperands`；这是
它测试代码操作时触发的内部问题，不是编辑器诊断。真正需要确认的是输出包含
`Loaded compilation database` 和 `Building AST`，且不再出现 `pp_file_not_found`
或 `drv_unknown_argument`。

如果升级或切换了内核，重新运行 `xmake f -c && xmake`，让编译数据库和
`/lib/modules/$(uname -r)/build` 保持一致。

= 驱动运行时的心智模型

PCIe 驱动通常不主动扫描总线。Linux PCI 核心先枚举硬件并创建 `struct pci_dev`，
然后拿设备 ID 与各驱动的 ID 表匹配：

#align(center)[
  #block(width: 82%, fill: rgb("#f4f6f8"), inset: 12pt, radius: 4pt)[
```text
PCIe 硬件
   │  Linux 启动时枚举
   ▼
struct pci_dev
   │  vendor/device ID 与驱动表匹配
   ▼
probe(pdev, id)
   │  初始化成功才返回 0
   ▼
驱动拥有设备；移除时调用 remove()
```
  ]
]

真正拥有设备的驱动通常会在 `probe()` 中启用设备、申请 BAR、映射 MMIO、设置
DMA、注册中断，并在失败路径或 `remove()` 中严格逆序释放。本教程有意停在这些
硬件操作之前。

= 第一课：亲手加入模块入口和出口

现在把 `src/pcie_lab.c` 替换为下面的代码：

```c
// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>

#define DRIVER_NAME "pcie_lab"

static int __init pcie_lab_init(void)
{
	pr_info(DRIVER_NAME ": module loaded\n");
	return 0;
}

static void __exit pcie_lab_exit(void)
{
	pr_info(DRIVER_NAME ": module unloaded\n");
}

module_init(pcie_lab_init);
module_exit(pcie_lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PCIe driver learner");
MODULE_DESCRIPTION("Linux PCIe driver lab");
MODULE_VERSION("0.1.0");
```

这里的职责很简单：

+ `__init` 标记只在初始化阶段使用的代码；初始化完成后内核可回收对应内存。
+ `__exit` 标记只在可卸载模块退出时使用的代码。
+ `module_init()` 与 `module_exit()` 指定加载和卸载入口。
+ `pr_info()` 写入内核日志，不会打印到执行 `insmod` 的终端。

编译检查点：

```bash
xmake
modinfo build/pcie_lab.ko
```

如果你在实验虚拟机中，可以打开两个终端。终端 A：

```bash
sudo dmesg --follow
```

终端 B：

```bash
sudo insmod ./build/pcie_lab.ko
lsmod | grep pcie_lab
sudo rmmod pcie_lab
```

这一步没有注册 PCI 驱动，也没有访问任何设备。

= 第二课：注册一个默认不匹配设备的 PCI 驱动

== 加入 PCI 头文件和 ID 宏

在两个现有 include 附近加入：

```c
#include <linux/errno.h>
#include <linux/pci.h>
```

`Kbuild` 已经通过编译参数提供下面两个宏：

```c
PCIE_LAB_VENDOR_ID
PCIE_LAB_DEVICE_ID
```

默认值都是 `0xffff`，因此不会匹配正常设备。以后可通过 xmake 配置，而不必修改
源码中的数字。

== 写第一个 probe

把下面的函数放在 `pcie_lab_init()` 之前：

```c
static int pcie_lab_probe(struct pci_dev *pdev,
			  const struct pci_device_id *matched_id)
{
	dev_info(&pdev->dev,
		 "probe: BDF=%s vendor=%04x device=%04x class=%06x\n",
		 pci_name(pdev), pdev->vendor, pdev->device, pdev->class);
	dev_info(&pdev->dev,
		 "matched table entry %04x:%04x\n",
		 (unsigned int)matched_id->vendor,
		 (unsigned int)matched_id->device);

	return -ENODEV;
}
```

重点是最后一行：返回 0 代表驱动已经成功接管设备；这里尚未初始化任何硬件，所以
必须返回 `-ENODEV`，明确拒绝绑定。

== 写 ID 表

紧跟在 `probe()` 后加入：

```c
static const struct pci_device_id pcie_lab_ids[] = {
	{ PCI_DEVICE(PCIE_LAB_VENDOR_ID, PCIE_LAB_DEVICE_ID) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, pcie_lab_ids);
```

`{ 0 }` 是表尾哨兵。`MODULE_DEVICE_TABLE()` 会生成模块 alias，帮助用户空间判断哪个
模块可能支持哪个设备。

== 描述 pci_driver

在 ID 表后加入：

```c
static struct pci_driver pcie_lab_driver = {
	.name = DRIVER_NAME,
	.id_table = pcie_lab_ids,
	.probe = pcie_lab_probe,
};
```

这个结构体回答三个问题：驱动叫什么、匹配谁、匹配后调用谁。由于 `probe()` 永远
不成功，此阶段不需要 `remove()`；等下一阶段真正返回 0 时，必须先实现清理函数。

== 修改 init 和 exit

把原先两个函数的函数体改为：

```c
static int __init pcie_lab_init(void)
{
	pr_info(DRIVER_NAME ": registering observe-only PCI driver\n");
	return pci_register_driver(&pcie_lab_driver);
}

static void __exit pcie_lab_exit(void)
{
	pci_unregister_driver(&pcie_lab_driver);
	pr_info(DRIVER_NAME ": unregistered\n");
}
```

编译并检查默认 alias：

```bash
xmake
modinfo -F alias build/pcie_lab.ko
```

预期能看到：

```text
pci:v0000FFFFd0000FFFFsv*sd*bc*sc*i*
```

= 第三课：只观察 Linux 已记录的 BAR

BAR（Base Address Register）描述设备暴露的 I/O 或内存资源。Linux 在枚举阶段已把
它们记录到 `pdev->resource[]`。读取这些结构体字段不等于读取设备寄存器。

在 `probe()` 的开头声明循环变量：

```c
int bar;
```

在返回 `-ENODEV` 之前加入：

```c
for (bar = 0; bar < PCI_STD_NUM_BARS; ++bar) {
	if (pci_resource_len(pdev, bar) == 0)
		continue;

	dev_info(&pdev->dev, "BAR%d: %pR\n", bar,
		 &pdev->resource[bar]);
}
```

代码只格式化 `struct resource`。它没有调用这些会改变所有权或开始硬件访问的函数：

```c
pci_enable_device(...);
pci_request_regions(...);
pci_iomap(...);
readl(...);
writel(...);
```

#callout("为什么暂时不写 remove", info)[
  因为当前 `probe()` 总是失败，PCI 核心不会认为驱动拥有设备，自然也不会对该设备
  调用 `remove()`。等你在专用虚拟设备上让 `probe()` 返回 0 之前，应先写好每一步
  初始化的反向清理路径。
]

= 查看设备并安全触发 probe

先观察系统设备：

```bash
lspci -nn
lspci -nnk
lspci -s 03:00.0 -nnk
```

示例输出：

```text
03:00.0 Ethernet controller [0200]: Example Corp Device [1234:5678]
```

- `03:00.0` 是 BDF，即 bus:device.function。
- `[1234:5678]` 是 Vendor ID 与 Device ID。
- `Kernel driver in use:` 表示设备已经被一个驱动拥有。

#callout("练习设备选择", caution)[
  不要选择系统盘控制器、显卡、正在联网的网卡、USB 控制器或 PCIe Root Port；不要
  为了触发教程向 sysfs 的 `unbind` 写值；不要使用 `PCI_ANY_ID`。最好使用虚拟机中
  专门添加的教学设备。
]

假设专用实验设备的 ID 是 `1234:5678`：

```bash
xmake f -c --pci_vendor=0x1234 --pci_device=0x5678
xmake
modinfo -F alias build/pcie_lab.ko
sudo insmod ./build/pcie_lab.ko
sudo dmesg | tail -n 30
sudo rmmod pcie_lab
```

只有设备尚未被其他驱动绑定时，PCI 核心才会尝试本驱动的 `probe()`。教程驱动最终
返回 `-ENODEV`，因此只会留下观察日志，不会拥有设备。

= 本阶段完整参考代码

如果前面逐段修改后无法编译，可对照下面的完整检查点。它只存在于文档中；请自己
定位差异并修改 `src/pcie_lab.c`。

```c
// SPDX-License-Identifier: GPL-2.0-only
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

#define DRIVER_NAME "pcie_lab"

static int pcie_lab_probe(struct pci_dev *pdev,
			  const struct pci_device_id *matched_id)
{
	int bar;

	dev_info(&pdev->dev,
		 "probe: BDF=%s vendor=%04x device=%04x class=%06x\n",
		 pci_name(pdev), pdev->vendor, pdev->device, pdev->class);
	dev_info(&pdev->dev, "matched table entry %04x:%04x\n",
		 (unsigned int)matched_id->vendor,
		 (unsigned int)matched_id->device);

	for (bar = 0; bar < PCI_STD_NUM_BARS; ++bar) {
		if (pci_resource_len(pdev, bar) == 0)
			continue;

		dev_info(&pdev->dev, "BAR%d: %pR\n", bar,
			 &pdev->resource[bar]);
	}

	dev_info(&pdev->dev,
		 "observe-only: returning -ENODEV, device not claimed\n");
	return -ENODEV;
}

static const struct pci_device_id pcie_lab_ids[] = {
	{ PCI_DEVICE(PCIE_LAB_VENDOR_ID, PCIE_LAB_DEVICE_ID) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, pcie_lab_ids);

static struct pci_driver pcie_lab_driver = {
	.name = DRIVER_NAME,
	.id_table = pcie_lab_ids,
	.probe = pcie_lab_probe,
};

static int __init pcie_lab_init(void)
{
	pr_info(DRIVER_NAME ": registering observe-only PCI driver\n");
	return pci_register_driver(&pcie_lab_driver);
}

static void __exit pcie_lab_exit(void)
{
	pci_unregister_driver(&pcie_lab_driver);
	pr_info(DRIVER_NAME ": unregistered\n");
}

module_init(pcie_lab_init);
module_exit(pcie_lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PCIe driver learner");
MODULE_DESCRIPTION("Observe-only Linux PCIe driver lab");
MODULE_VERSION("0.2.0");
```

= 常见问题

== clangd 仍然说 linux/init.h 不存在

依次检查：

```bash
test -f compile_commands.json
test -f /lib/modules/$(uname -r)/build/Makefile
clangd --check=src/pcie_lab.c --compile-commands-dir=.
```

然后重启编辑器中的 clangd。不要把用户态 `/usr/include/linux` 当成内核模块头文件。
若只在 `--check` 末尾看见 `tweak: SwapBinaryOperands`，参见第 3.2 节的说明。

== xmake 能编译，但编辑器还是红线

这通常说明编辑器启动了另一个 clangd、工作目录不在本项目，或者语言服务器还缓存
着旧诊断。以 `clangd --check` 的输出为基准，再检查编辑器的 LSP 日志。

== Invalid module format

比较当前内核与模块的 vermagic：

```bash
uname -r
modinfo -F vermagic ./build/pcie_lab.ko
```

不一致时重新运行：

```bash
xmake f -c
xmake -r
```

== 加载成功但没有 probe 日志

+ 完成版代码是否已经加入 ID 表与 `pci_register_driver()`；
+ `modinfo -F alias` 中是不是目标 ID；
+ `lspci -nnk` 中 ID 是否一致；
+ 目标是否已经显示 `Kernel driver in use`；
+ 改 ID 后是否重新构建并重新加载模块。

= 下一步怎样继续

完成本教程后，可按下面顺序推进：

+ 给模块增加只控制日志级别的 `module_param`。
+ 打印 `subsystem_vendor`、`subsystem_device`。
+ 用 `pci_read_config_byte()` 读取标准配置头中的 Revision ID。
+ 在 QEMU 专用教学设备上实现 `remove()`，然后才让 `probe()` 返回 0。
+ 学习 `pci_enable_device()`、`pci_request_regions()` 和 `pci_iomap()`，每一步都写好
  错误回滚与反向清理。
+ 最后才学习 MSI/MSI-X 与 DMA，因为它们同时涉及并发、内存屏障和设备停止顺序。

下一阶段应在可恢复虚拟环境中完成这条路径：

```text
匹配 -> enable -> request BAR -> iomap -> 读已知只读寄存器
     -> iounmap -> release BAR -> disable
```
