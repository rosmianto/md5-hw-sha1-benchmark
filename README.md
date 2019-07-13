# MD5 vs Hardware Accelerated SHA1 benchmark
A simple comparison of two hashing algorithms

## Setup
Arduino-ESP32 with 240MHz default freq

### --- MD5 (dual core) ---

8129978 bytes total

CPU1 2fdd884f7c3dd9990fd6471e5d7c045d
CPU1 61547 ms (4064989 bytes)
CPU1 66.05 bytes/ms

CPU0 79e7cda9c91fbbeab739f04e19a6dca6
CPU0 61547 ms (4064989 bytes)
CPU0 66.05 bytes/ms

Total throughput:
132 bytes/ms

### --- SHA1 (1KB buffer) ---
8129978 bytes total
8874E15149792F5E426C29EE0F4E36C27B5737C2
23854 ms
340.00 bytes/ms

### --- SHA1 (2KB buffer) ---
8129978 bytes total
8874E15149792F5E426C29EE0F4E36C27B5737C2
23836 ms
341.00 bytes/ms
