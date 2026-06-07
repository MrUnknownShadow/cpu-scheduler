# CPU Scheduler

C-based CPU scheduling simulator implementing **FCFS**, **Round Robin**, and **SRTF**.

## Input Format

`input.txt` — space-separated columns:
```
PID  ArrivalTime  BurstTime
```

## Build
```bash
make
```

## Usage
```bash
./scheduler input.txt fcfs
./scheduler input.txt rr 4
./scheduler input.txt srtf
```

## Output
Gantt-style chart followed by per-process turnaround and waiting times plus averages.
