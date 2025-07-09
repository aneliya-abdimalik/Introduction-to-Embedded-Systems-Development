# Introduction-to-Embedded-Systems-Development

This repository contains three key embedded projects for the PIC18F8722 microcontroller, showcasing:
- **Round-Robin Scheduling** (no interrupts)
- **Interrupt-Driven I/O** (timer & external interrupts)
- **USART & ADC Communication** with a PC simulator

## Technology Stack
- **Hardware:** PIC18F8722 development board
- **IDE & Compiler:** MPLAB X IDE 5.45, XC8 v2.30
- **Language:** Embedded C / Assembly
- **Tools:** make, MPLAB Simulator, PICkit ICD programmer
- **PC Simulator:** Python 3.10+ (`pyserial`, `pygame`)

## Projects Overview

1. **Round-Robin LED Sequencer** (`hw1/`)
   - **Stack:** PIC main loop, PORTC LEDs, GPIO inputs
   - **Description:** Implements a software scheduler blinking LEDs at 500 ms intervals, with button-driven sequence control. No timers or interrupts—pure round-robin design.

2. **Hungry Hippo LED Game** (`hw2/`)
   - **Stack:** TIMER0 interrupt (5 ms), INT0 external interrupt, 7‑segment display
   - **Description:** A real‑time game where a “hippo” LED climbs to catch a blinking “prize.” Demonstrates interrupt handling, debouncing, soft/hard resets, and score decay on seven‑segment displays.

3. **Parking Lot Simulator** (`hw3/`)
   - **Stack:** USART (115200 bps, 8N1), ADC (AN12), Python PC simulator
   - **Description:** Interfaces with a Python GUI to manage a multilevel parking lot. PIC sends status messages every 100 ms, reads level selection via ADC, and toggles display modes with a button.

