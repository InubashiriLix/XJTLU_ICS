#!/usr/bin/env python3
"""Generate small Verilog testbench templates for RTL modules."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path


IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_$]*")
MODULE_RE = re.compile(r"\bmodule\s+([A-Za-z_][A-Za-z0-9_$]*)\b")


@dataclass
class Port:
    direction: str
    name: str
    width: str


@dataclass
class Module:
    name: str
    path: Path
    params: list[str]
    ports: list[Port]


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//.*", "", text)


def skip_ws(text: str, index: int) -> int:
    while index < len(text) and text[index].isspace():
        index += 1
    return index


def read_balanced(text: str, index: int) -> tuple[str, int]:
    if index >= len(text) or text[index] != "(":
        raise ValueError("expected '('")

    depth = 0
    start = index + 1
    i = index
    while i < len(text):
        char = text[i]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return text[start:i], i + 1
        i += 1

    raise ValueError("unbalanced parentheses")


def split_top_level_commas(text: str) -> list[str]:
    items: list[str] = []
    depth = 0
    start = 0

    for i, char in enumerate(text):
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif char == "," and depth == 0:
            items.append(text[start:i].strip())
            start = i + 1

    tail = text[start:].strip()
    if tail:
        items.append(tail)
    return items


def parse_params(text: str) -> list[str]:
    params: list[str] = []
    for item in split_top_level_commas(text):
        match = re.search(
            r"\bparameter\b(?:\s+(?:integer|real|signed|unsigned|wire|reg))*"
            r"\s*(?:\[[^\]]+\]\s*)?([A-Za-z_][A-Za-z0-9_$]*)\s*=",
            item,
        )
        if match:
            params.append(match.group(1))
    return params


def parse_ports(text: str) -> list[Port]:
    ports: list[Port] = []

    for item in split_top_level_commas(text):
        match = re.search(r"\b(input|output|inout)\b", item)
        if not match:
            continue

        direction = match.group(1)
        width_match = re.search(r"(\[[^\]]+\])", item)
        width = width_match.group(1) if width_match else ""

        before_default = item.split("=", 1)[0]
        names = IDENT_RE.findall(before_default)
        if not names:
            continue

        name = names[-1]
        if name in {"input", "output", "inout", "wire", "reg", "logic", "signed", "unsigned"}:
            continue
        ports.append(Port(direction=direction, name=name, width=width))

    return ports


def parse_modules(path: Path) -> list[Module]:
    text = strip_comments(path.read_text())
    modules: list[Module] = []

    for match in MODULE_RE.finditer(text):
        name = match.group(1)
        index = skip_ws(text, match.end())

        params: list[str] = []
        if index < len(text) and text[index] == "#":
            index = skip_ws(text, index + 1)
            try:
                param_text, index = read_balanced(text, index)
            except ValueError:
                continue
            params = parse_params(param_text)

        index = skip_ws(text, index)
        if index >= len(text) or text[index] != "(":
            continue

        try:
            port_text, _ = read_balanced(text, index)
        except ValueError:
            continue

        modules.append(Module(name=name, path=path, params=params, ports=parse_ports(port_text)))

    return modules


def override_for(param: str, params: set[str]) -> str | None:
    if param == "PERIOD_COUNT":
        return "16"
    if param == "COUNT_MAX":
        return "4"
    if param == "COUNT_WIDTH":
        return "5" if "PERIOD_COUNT" in params else "4"
    if param == "CLOCK_HZ":
        return "100"
    if param == "PWM_HZ":
        return "10"
    return None


def signal_decl(port: Port) -> str:
    kind = "reg" if port.direction == "input" else "wire"
    width = f" {port.width}" if port.width else ""
    init = " = 0" if port.direction == "input" else ""
    return f"    {kind}{width} {port.name}{init};"


def is_clock(name: str) -> bool:
    return name in {"clk", "clock"} or name.endswith("_clk")


def is_reset_low(name: str) -> bool:
    return name in {"rst_n", "reset_n"} or name.endswith("_rst_n") or name.endswith("_reset_n")


def is_reset_high(name: str) -> bool:
    return name in {"rst", "reset"} or name.endswith("_rst") or name.endswith("_reset")


def drive_statement(port: Port) -> list[str]:
    name = port.name
    lower = name.lower()
    one = "1'b1" if not port.width else "'h1"
    zero = "1'b0" if not port.width else "'h0"

    if is_clock(lower) or is_reset_low(lower) or is_reset_high(lower):
        return []
    if "btn" in lower or "key" in lower:
        return [f"        #30 {name} = {one};", f"        #100 {name} = {zero};"]
    if "toggle" in lower or "valid" in lower or "enable" in lower or lower.endswith("_en"):
        return [
            f"        #30 {name} = {one};",
            f"        #10 {name} = {zero};",
            f"        #40 {name} = {one};",
            f"        #10 {name} = {zero};",
        ]
    if "duty" in lower:
        return [f"        #20 {name} = 8;"]
    return [f"        #30 {name} = {one};", f"        #50 {name} = {zero};"]


def render_tb(module: Module, sim_dir: Path) -> str:
    tb_name = f"{module.name}_tb"
    param_set = set(module.params)
    overrides = [(param, override_for(param, param_set)) for param in module.params]
    overrides = [(param, value) for param, value in overrides if value is not None]

    lines: list[str] = [
        "`timescale 1ns/1ps",
        "",
        f"module {tb_name};",
        f"    // Generated by tools/gen_tb.py from {module.path.as_posix()}",
    ]

    if overrides:
        lines.append("")
        for param, value in overrides:
            lines.append(f"    localparam integer {param} = {value};")

    if module.ports:
        lines.append("")
        lines.extend(signal_decl(port) for port in module.ports)

    clocks = [port for port in module.ports if port.direction == "input" and is_clock(port.name.lower())]
    if clocks:
        lines.append("")
        for port in clocks:
            lines.append(f"    always #5 {port.name} = ~{port.name};")

    lines.append("")
    if overrides:
        lines.append(f"    {module.name} #(")
        for index, (param, _) in enumerate(overrides):
            comma = "," if index + 1 < len(overrides) else ""
            lines.append(f"        .{param}({param}){comma}")
        lines.append("    ) dut (")
    else:
        lines.append(f"    {module.name} dut (")

    for index, port in enumerate(module.ports):
        comma = "," if index + 1 < len(module.ports) else ""
        lines.append(f"        .{port.name}({port.name}){comma}")
    lines.append("    );")

    lines.extend(
        [
            "",
            "    initial begin",
            f'        $dumpfile("{sim_dir.as_posix()}/{module.name}.vcd");',
            f"        $dumpvars(0, {tb_name});",
            "",
            "        #20;",
        ]
    )

    for port in module.ports:
        lower = port.name.lower()
        if port.direction == "input" and is_reset_low(lower):
            lines.append(f"        {port.name} = 1'b1;")
        elif port.direction == "input" and is_reset_high(lower):
            lines.append(f"        {port.name} = 1'b0;")

    stimulus: list[str] = []
    for port in module.ports:
        if port.direction == "input":
            stimulus.extend(drive_statement(port))

    if stimulus:
        lines.append("")
        lines.extend(stimulus)

    lines.extend(["", "        #200;", "        $finish;", "    end", "endmodule", ""])
    return "\n".join(lines)


def discover_modules(rtl_dir: Path) -> list[Module]:
    modules: list[Module] = []
    for path in sorted(rtl_dir.rglob("*.v")) + sorted(rtl_dir.rglob("*.sv")):
        modules.extend(parse_modules(path))
    return modules


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rtl-dir", type=Path, required=True)
    parser.add_argument("--tb-dir", type=Path, required=True)
    parser.add_argument("--sim-dir", type=Path, required=True)
    parser.add_argument("--force", action="store_true", help="overwrite existing testbenches")
    parser.add_argument("--list", action="store_true", help="list discovered modules without writing files")
    args = parser.parse_args()

    modules = discover_modules(args.rtl_dir)
    if args.list:
        for module in modules:
            print(f"{module.name}\t{module.path}")
        return 0

    args.tb_dir.mkdir(parents=True, exist_ok=True)
    args.sim_dir.mkdir(parents=True, exist_ok=True)

    for module in modules:
        target = args.tb_dir / f"{module.name}_tb.v"
        if target.exists() and not args.force:
            print(f"[SKIP] {target} exists")
            continue

        target.write_text(render_tb(module, args.sim_dir))
        print(f"[GEN]  {target}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
