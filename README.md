# FreeRTOS PC Scheduler Simulation

This project simulates the FreeRTOS real-time operating system kernel in a PC (Linux/POSIX) environment. It demonstrates a custom scheduling algorithm without requiring embedded hardware.

## Overview

The simulation implements a **4-level priority-based scheduler**:

*   **Priority 0 (Real-Time):** FCFS (First-Come First-Served). Runs to completion without interruption.
*   **Priority 1-3 (User Tasks):** MLFQ (Multi-Level Feedback Queue). Tasks have a 1-second time quantum. If a task exceeds its quantum, its priority drops, and it moves to a lower priority queue.
*   **Timeout:** Any task pending in the system for more than 20 seconds is automatically terminated.

## Build & Run

### Prerequisites
*   GCC
*   Make

### Compilation
Build the project using `make`:

```bash
make
```

### Usage
Run the simulation by providing a task list file (e.g., `tasks.txt`):

```bash
./freertos_sim tasks.txt
```

**Input File Format (`tasks.txt`):**
Each line represents a task: `Arrival Time, Priority, CPU Time`
```
0, 1, 5
2, 0, 3
```

### Clean
```bash
make clean
```
