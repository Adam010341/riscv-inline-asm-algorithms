# RISC-V Inline Assembly Algorithms

![RISC-V](https://img.shields.io/badge/ISA-RV64IF-283272?logo=riscv&logoColor=white)
![C](https://img.shields.io/badge/language-C%20%2B%20GCC%20inline%20asm-00599C?logo=c&logoColor=white)
![Spike](https://img.shields.io/badge/simulator-Spike%20%2B%20pk-555)

Insertion sort, binary search and linked-list cycle detection, written in RV64I + F assembly and embedded in C with GCC `asm volatile`. Cross-compiled with the RISC-V GNU toolchain and run on [Spike](https://github.com/riscv-software-src/riscv-isa-sim) with the proxy kernel.

> Lab 1 of Computer Organization (NCKU CSIE, Spring 2026). See the [lab series](#lab-series) below.

All algorithm logic is inside `asm volatile(...)`; the surrounding C only does I/O. It uses GCC operand constraints (`"+r"`, `"r"`, `"f"`), clobber lists (temporaries, FP registers, `"memory"`), `flt.s` / `feq.s` for single-precision comparison, `ld` for pointer chasing, and local numeric labels (`1:` / `3f` / `1b`) so the block can be inlined in a loop.

## Problems

| # | Problem | File | Key instructions |
|---|---------|------|------------------|
| 1 | Insertion sort on `float[]`, returning the number of shifts | [`src/1_insertion_sort.c`](src/1_insertion_sort.c) | `flw`, `fsw`, `flt.s`, `bltz` |
| 2 | Binary search on a sorted `float[]` | [`src/2_binary_search.c`](src/2_binary_search.c) | `feq.s`, `flt.s`, `srli` |
| 3 | Detect a cycle in a linked list and report the meeting node | [`src/3_linked_list_cycle.c`](src/3_linked_list_cycle.c) | `ld`, `beq`, `beqz` |

- **Insertion sort:** the inner `while (j >= 0 && A[j] > key)` loop is in assembly. `&A[j]` is computed once and walked down 4 bytes per iteration. `flt.s` tests `key < A[j]`; while true, `A[j]` is stored to `4(t1)` (`A[j+1]`) and `shift_cnt` is incremented.
- **Binary search:** `left` / `right` live in `t0` / `t1`, `mid = (left + right) >> 1`. `feq.s` checks for a hit, otherwise `flt.s` picks the half. The result stays `-1` unless the "found" label is reached.
- **Cycle detection:** Floyd's tortoise and hare. `slow` advances one node (`ld slow, 8(slow)`), `fast` two. Offset 8 is the `next` field under the RV64 LP64 ABI. O(n) time, O(1) memory.

## Layout

```
src/
├── 1_insertion_sort.c
├── 2_binary_search.c
├── 3_linked_list_cycle.c
├── Makefile                # cross-compile + judge targets
└── judge{1,2,3}.conf       # local-judge configs
testcases/
├── input/
└── expected/
```

## Build and run

Needs `riscv64-unknown-linux-gnu-gcc`, `spike` and `pk`. The course Docker image has them:

```bash
docker run -it --name co-lab1 -v "$(pwd)":/workspace docker.io/asrlab/comp-org:pa0
```

```bash
cd src
make                    # builds executables 1, 2, 3 (static, RV64)
spike --isa=RV64GCV $RISCV/riscv64-unknown-linux-gnu/bin/pk 1 ../testcases/input/1_1.txt
make judge              # runs every public testcase through local-judge
```

## Lab series

| Lab | Repository | Topic |
|-----|------------|-------|
| 1 | **riscv-inline-asm-algorithms** (this repo) | RV64IF inline assembly |
| 2 | [rvv-mel-spectrogram](https://github.com/Adam010341/rvv-mel-spectrogram) | RISC-V Vector (RVV) intrinsics, FFT, DSP |
| 3 | [cache-aware-riscv-optimization](https://github.com/Adam010341/cache-aware-riscv-optimization) | Tree-PLRU cache simulator, cache-blocked transpose, RVV GEMM |

The test harness, I/O scaffolding and testcases were provided by the course staff. The assembly inside `asm volatile(...)` is my own work.
