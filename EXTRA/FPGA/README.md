# About This Extra Content

## What is FPGA?

- FPGA (Field Programmable Gate Array) is a type of integrated circuit (IC) that is designed to be reconfigurable and programmable.
- Just Think about that: you can use language to describe the circuit, instead of drawing it out. About the layout, bram, etc, the synthesis tool will do the job.

## What benefits of FPGA?

- Build the wheels if you want or learn
  - for embeded developers who used to use microcontrollers, linux, windows, etc. You guys are heavily dependent on the hardware, or let's say chip support.
  - but what if you find out that the chip you are using is not support some features you need?
  - let's say, you got an stm32f103c8t6 minimum dev board, but you need to use the fd-can to control a motor, but it does not support it. Luckily, you got an FPGA dev board, then you can use it to build the fd-can driver and use other way like spi to connect the fpga and stm32, so that you can control the motor on your stm32
  - You're a engineer, not a baby. If there is no support, build it yourself.
- MCU / CPU can be SLOWER than FPGA!
  - fpga can implement parallel computing natrually because that how the circuit works, which, MCU can not, because it is a serial computing.
  - another key point is that, MCU have to execute the command in order, like this:

  ```asm
  ; this is a simple addition computing example
  mov r0, 0x1234
  mov r1, 0x5678
  add r0, r1
  ```

  while fpga can execute in one time: like

  ```verilog
  // a, b, c are all registers in fpga,
  logic [31:0] a = 32'h1234;
  logic [31:0] b = 32'h5678;
  logic [31:0] out;
  always_ff @(posedge clk) begin
    // in always_ff, out = a + b is triggered when clk changes from low to high
      out <= a + b;
  end
  ```

  which means, the fpga can execute in fast speed, and parallel, which cpu (mcu) can not.

  > No offense for embeded developers, Actually, in most of the cases, a well-supported mcu develop board can be easy to write code and to be honest, although we said fpga can be faster, but now high frequency mcu can do the same.
  > another point is that, verilog / systemverilog is a language describing the hardware, not like c / cpp. DO NOT WRITE VERILOG LIKE C / CPP!
  > verilog has little support for abstraction, which is a good thing in software development, but in hardware development, it is not a good idea.
  > sometimes you have to drop some unsuitable idea you learnt from software development ...

### Some Applications?

- FPGA can capture the single, while these singals may too fast for MCU to capture. that is, FPGA is a good way to develop sensor.
- FPGA's parallel computing enable it to fasten the computing speed, like, using FPGA to implement a neuron network, which is a good research way.
- Actually, you can use FPGA to implement a CPU / MCU.

## What is Tang Nano?

- Tang Nano is a tiny FPGA board, which is used to learn or evaluate FPGA.
- kinda cheap (really?) I bought TangNano1K by 18RMB, and TangNano9K by 66RMB. (now it is more expensive, fuck)
- it has a 27MHz clock, and a 16MB of RAM, which is enough for most of the cases.

## OUR LECENSE

> UNDER CONSTRUCTION, MAY UPDATE INDEX IN THE FUTURE
> [Index Page](./index.md)

## Proj Structure

Well, the `Tang1K/` and `Tang9K/` are my history projects collections, but good enough to be a template / starter project.\
I didn't use official IDE / tools, but use nvim, verible, slang-server, openFPGALoader, yosys, gtkwave\
kinda weird, but it works good for me.\
if you want to use it, go install a wsl / linux, and try to learn how to use nvim and how to configure the tools.\

Specific Project README can be found inside.\
here is the links\

- [TangNano9K Template](./Tang9K/template/README.md)
- [TangNano1K Template](./Tang1K/template/README.md)
