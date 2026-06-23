# Producer-Consumer Simulation (C++ / POSIX Threads)

A multi-threaded bounded-buffer implementation of the classic Producer-Consumer synchronization problem using POSIX threads, mutex locks, and semaphores.

## Concepts Demonstrated
- **POSIX Threads (`pthread`)** — concurrent producer and consumer threads
- **Mutex (`pthread_mutex_t`)** — protects the shared buffer (critical section)
- **Counting Semaphores (`sem_t`)** — `empty_slots` and `filled_slots` coordinate access without busy-waiting
- **Deadlock Prevention** — strict lock ordering: always `sem_wait` before `mutex_lock`
- **Race Condition Prevention** — all buffer reads/writes enclosed in mutex lock/unlock

## How It Works
- 3 producer threads each produce 4 items → buffer (max size 5)
- 3 consumer threads each consume 4 items ← buffer
- Producers block when buffer is full; consumers block when buffer is empty

## Build & Run
```bash
g++ -o producer_consumer producer_consumer.cpp -lpthread
./producer_consumer
```
> Requires Linux/macOS or WSL on Windows.

## Sample Output
```
=== Producer-Consumer Simulation (Buffer=5) ===

[Producer 1] Produced item 1 (val=83) | Buffer size: 1
[Producer 2] Produced item 2 (val=47) | Buffer size: 2
[Consumer 1] Consumed item  (val=83) | Buffer size: 1
...
=== Simulation complete. No deadlocks detected. ===
```

## Key Design Decision
Using **two semaphores + one mutex** (rather than mutex alone) allows producers and consumers to sleep efficiently when blocked, avoiding CPU-wasting spin loops — a standard pattern in OS-level concurrent system design.
