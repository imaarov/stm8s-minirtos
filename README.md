# STM8S MiniRTOS

a super minimal preemptive real-time operating system for STM8S103F3 microcontroller, written in C and STM8 asm, using SDCC.

## Features

- Preemptive multitasking
- static task creation
- individual and isolate task stacks
- ms system tick
- timer based scheduling using TIM4 peripheral

## Hardware and toolchain

- **MCU:** STM8S103F3
- **Compiler:** SDCC
- **Timer:** TIM4
- **Development enviornment:** VSCode/EDIE (you can use stm8flash for flash, but eide was good for me, mainly because theres no good ide for stm8 in GNU/Linux)


## Example

The `examples/led_tasks` directory contains application wiht three tasks:
- button polling and debounce
- led heartbeat
- periodic cpu-intensive task (simulating by busy wait loop)

## Status

experimental project. intended for learning and experimentation with embdded systems, task scheduling, the scheduler, context switching.
