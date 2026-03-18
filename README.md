# STM32 CAN Protocol Implementation

This repository contains two CAN (Controller Area Network) projects targeting STM32 microcontrollers — a custom register-level bxCAN protocol implementation on the STM32F412, and a hardware CAN demo on the STM32F103.

---

## Repository Structure

```
stm32-can-protocol-implementation/
├── can-protocol-implementation/   # Original bxCAN protocol implementation (STM32F412)
│   ├── main.c                     # Core protocol logic — mailboxes, FIFO, TX/RX
│   ├── CAN_setup.h                # Custom structs — CAN_ref, TX/RX mailbox, FIFO, message
│   ├── system_stm32f4xx.c         # STM32F4xx system initialization
│   └── CAN_protocol_implementation_report.pdf  # Full implementation report
└── stm32-can-demo/                # CAN peripheral demo on STM32F103 (Keil MCBSTM32 board)
    ├── CanDemo.c                  # Main demo — ADC read, CAN TX/RX, LCD display
    ├── CAN.c                      # CAN adaptation layer
    ├── CAN.h                      # CAN message struct and function prototypes
    └── Abstract.txt               # Project description and clock configuration
```

---

## Project 1 — bxCAN Protocol Implementation (STM32F412)

A register-level implementation of the **STM32 bxCAN (Basic Extended CAN)** peripheral, modelling the full transmit and receive pipeline as described in the STM32 bxCAN specification.

### What's Implemented

**Data Structures (`CAN_setup.h`)**
- `message` — CAN frame struct with payload, length, and validity flag
- `FIFO` — custom FIFO queue for the receiver mailboxes using dynamic memory allocation
- `TX_mailbox` / `RX_mailbox` — transmit and receive mailbox structs with state tracking
- `CAN_ref` — top-level CAN controller struct mirroring hardware registers (`CAN_MCR`, `CAN_MSR`, `CAN_TSR`, `CAN_RF0R`, `CAN_RF1R`, `CAN_IER`, `CAN_ESR`, `CAN_BTR`, `CAN_TI0R`)

**Protocol Logic (`main.c`)**
- `CAN_init()` — initialises all CAN registers and mailboxes to reset state per bxCAN spec
- `CAN_reset()` — resets the CAN controller if the reset request bit is set
- `transmit_push()` — implements the 3-mailbox TX arbitration state machine (Idle → Pending → Scheduled → Transmit)
- `activate_receive_message_box0/1()` — RX mailbox state machines handling message reception, FIFO overflow, and release flags
- FIFO operations: `push()`, `pop()`, `peek()`, `isFull()`, `isEmpty()`

### Target Hardware
- **MCU:** STM32F412ZGTx
- **Toolchain:** Keil MDK (µVision)
- **CAN Speed:** Configured via `CAN_BTR` register

### Report
See `CAN_protocol_implementation_report.pdf` for the full design report including register configuration, state machine diagrams, and implementation details.

---

## Project 2 — CAN Hardware Demo (STM32F103 / MCBSTM32)

A hardware demonstration of the STM32F103 CAN peripheral using the Keil MCBSTM32 evaluation board. Reads an analog value via ADC, transmits it over CAN, receives it back, and displays both values on an LCD.

> **Note:** This project is based on the Keil MCBSTM32 CAN example (© 2005–2007 Keil Software) and is included here as a reference hardware demo alongside the original protocol implementation above.

### Functionality
- **Clock:** SYSCLK = 72 MHz, CAN speed = 500 kbit/s
- **ADC:** Reads analog input on PA1 (Channel 1), scaled to 8 bits
- **CAN TX:** Transmits ADC value in first byte of a standard CAN frame (ID = 33)
- **CAN RX:** Receives the message back (loopback/silent test mode by default)
- **LCD:** Displays transmitted and received hex values in real time

### Switching to Real CAN Network
To connect to an actual CAN bus, comment out the loopback line in `CanDemo.c`:
```c
// CAN_testmode(CAN_BTR_SILM | CAN_BTR_LBKM);
```

### Target Hardware
- **MCU:** STM32F103RBT6
- **Board:** Keil MCBSTM32 Evaluation Board
- **Toolchain:** Keil µVision

---

## Dependencies

- Keil MDK / µVision IDE
- STM32F4xx / STM32F10x Standard Peripheral Library
- MCBSTM32 board (for Project 2 hardware demo)

---

## Key Concepts Demonstrated

- Register-level bxCAN peripheral programming in C
- TX arbitration across 3 hardware mailboxes
- RX FIFO management with overflow and release handling
- Custom data structures for embedded message queuing
- ADC peripheral configuration and CAN frame construction
- Hardware-software integration on STM32 platform
