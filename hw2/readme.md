# Real-Time Embedded Game Controller

## Overview

This project implements an interrupt-driven, real-time game on the **PIC18F8722** microcontroller. The game mimics a "Hungry Hippo"-like mechanic using a **LED strip** (PORTD) and a **4-digit 7-segment display** (PORTH and PORTJ). Game logic is driven by **TIMER0** interrupts and user input via **PORTB0**. The system uses timers for dynamic behavior, including gravity simulation, prize blinking, scoring, and display multiplexing.

## Features

- Timer-driven real-time game loop
- LED strip gameplay logic (hippo movement and prize tracking)
- External interrupt for user input
- 7-segment display for total score
- Gravity and blinking effects via `TIMER0`
- Soft and hard reset behavior
- Responsive display rendering using interrupt multiplexing
- Fully implemented using Embedded C for PIC18F8722

## Technologies Used

- Embedded C (XC8)
- MPLAB X IDE / Simulator
- PIC18F8722 microcontroller
- PORTD (LED game board)
- PORTB0 (Interrupt-driven user input)
- PORTH and PORTJ (7-segment display control)
- TIMER0 and `INT0` interrupts

## Game Description

- The **hippo** (player) starts at the bottom (RD7) with a size of 1 LED.
- The **prize** is always on top (RD0) and blinks every 500ms.
- User presses the input button (PORTB0) to move the hippo up.
- **Gravity** pulls the hippo down every 350ms.
- If the hippo reaches the prize:
  - Player earns `round_score` (max 100, decreases by 10 every second).
  - Hippo grows by 1 (up to 5).
  - Game soft resets and total score is updated.
- When hippo reaches size 6, it hard resets to size 1 (but score is preserved).
- Score is rendered on a **4-digit 7-segment display**.

## Hardware Setup

- **LED Display (PORTD)**: Represents vertical game strip.
- **User Input (PORTB0)**: Triggered via `INT0` on rising edge.
- **7-Segment Display (PORTH + PORTJ)**:
  - PORTH[3:0] selects digits D0–D3.
  - PORTJ[6:0] drives segments A–G.
  - Display updated via multiplexing in the timer ISR.

## Timing and Interrupts

| Feature            | Trigger Interval | Description                      |
|--------------------|------------------|----------------------------------|
| Prize Blink        | 500ms            | Toggles RD0                      |
| Gravity Pull       | 350ms            | Moves hippo down one LED         |
| Round Score Decay  | 1000ms           | Decreases by 10 points per sec   |
| Timer ISR          | 5ms              | Drives all events & display      |
| Display Refresh    | 5ms              | Cycles through 4 digits          |

## Soft Reset Logic

- All PORTD LEDs blink in a 2-second sequence:  
  - On-Off-On-Off-On (400ms each)
- Hippo size increases by 1
- Hippo repositions to the bottom
- Round score resets to 100

## Hard Reset Logic

- Triggered when hippo reaches size 6
- Hippo size resets to 1
- Total score remains intact
- Game resumes with updated hippo

## File Structure

- `main.c`: Full game implementation for PIC18F8722 (interrupts, logic, display)
- `hw2-2.pdf`: Assignment specifications and hardware interface details

## Build & Deploy

1. Open MPLAB X IDE
2. Set target device to `PIC18F8722`
3. Use **XC8** compiler
4. Import `main.c` and configure project settings
5. Compile and upload to development board
6. Monitor PORTD and 7-segment display for gameplay

## Notes

- Game runs indefinitely
- Score display supports up to 9999 (no overflow handling)
- Responsive LED and display timing achieved via ISR, not polling
- Uses only `TIMER0` and `INT0` as per constraints

## License

Developed for educational purposes at METU CENG336 — Introduction to Embedded Systems.

