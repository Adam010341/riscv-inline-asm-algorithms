# RISC-V Inline Assembly Algorithms

![RISC-V](https://img.shields.io/badge/ISA-RV64IF-283272?logo=riscv&logoColor=white)
![C](https://img.shields.io/badge/language-C%20%2B%20GCC%20inline%20asm-00599C?logo=c&logoColor=white)
![Spike](https://img.shields.io/badge/simulator-Spike%20%2B%20pk-555)

Three classic algorithms — **insertion sort**, **binary search** and **linked-list cycle detection** —
hand-written in **RV64I + F-extension assembly** and embedded into C through GCC extended inline
assembly (`asm volatile`). The programs are cross-compiled with the RISC-V GNU toolchain and run on
the [Spike](https://github.com/riscv-software-src/riscv-isa-sim) ISA simulator with the RISC-V proxy kernel.

> Lab 1 of my *Computer Organization* coursework (NCKU CSIE, Spring 2026).
> See the [full lab series](#lab-series) below.

## Highlights

- Every line of algorithm logic lives inside `asm volatile(...)`; the surrounding C only does I/O.
- Correct use of GCC **operand constraints** (`"+r"`, `"r"`, `"f"`) and **clobber lists**
  (temporaries, FP registers and `"memory"`), so the compiler can safely schedule around the asm block.
- Single-precision floating-point comparison with `flt.s` / `feq.s`, and 64-bit pointer chasing with `ld`.
- Local numeric labels (`1:` / `3f` / `1b`) so the asm block can be inlined into a loop without symbol clashes.

## Problems and approach

| # | Problem | File | Key instructions |
|---|---------|------|------------------|
| 1 | Insertion sort on `float[]`, returning the number of shifts | [`src/1_insertion_sort.c`](src/1_insertion_sort.c) | `flw`, `fsw`, `flt.s`, `bltz` |
| 2 | Binary search on a sorted `float[]` | [`src/2_binary_search.c`](src/2_binary_search.c) | `feq.s`, `flt.s`, `srli` |
| 3 | Detect a cycle in a linked list and report the meeting node | [`src/3_linked_list_cycle.c`](src/3_linked_list_cycle.c) | `ld`, `beq`, `beqz` |

### 1. Insertion sort

The C loop supplies `i` and `j = i - 1`. The inner `while (j >= 0 && A[j] > key)` loop is in assembly:

- `key` is loaded once into `ft0`. The address `&A[j]` is computed once (`slli` + `add`) and then
  **walked downward by 4 bytes** each iteration, so the index is never recomputed inside the loop.
- `flt.s t2, ft0, ft1` evaluates `key < A[j]`. While it holds, `A[j]` is stored to `4(t1)` (that is,
  `A[j+1]`) and `shift_cnt` is incremented.
- On exit, `key` is written to `A[j+1]` with the same `4(t1)` offset.

### 2. Binary search

- `left` / `right` are kept in `t0` / `t1`. `mid = (left + right) >> 1` and `&A[mid]` is formed with `slli` + `add`.
- `feq.s` checks for a hit first. Otherwise `flt.s target, A[mid]` picks the half to keep.
- The result register stays `-1` unless the "found" label is reached. This covers the empty and
  not-found edge cases without extra branches.

### 3. Linked-list cycle (Floyd's tortoise and hare)

- `slow` moves one node per step (`ld slow, 8(slow)`) and `fast` moves two. Offset `8` is the
  `next` field under the RV64 LP64 ABI (`int val` is padded to 8 bytes).
- `fast == NULL` or `fast->next == NULL` means there is no cycle. `slow == fast` means there is one,
  and that node is returned through the `meet` output operand.
- This runs in O(n) time and O(1) extra memory.

## Repository layout

```
.
├── src/
│   ├── 1_insertion_sort.c      # inline-asm implementation
│   ├── 2_binary_search.c
│   ├── 3_linked_list_cycle.c
│   ├── Makefile                # cross-compile + judge targets
│   └── judge{1,2,3}.conf       # local-judge configs
└── testcases/
    ├── input/                  # public testcases
    └── expected/               # expected outputs
```

## Build and run

You need `riscv64-unknown-linux-gnu-gcc`, `spike` and `pk`. The easiest way to get them is the course
Docker image:

```bash
docker run -it --name co-lab1 -v "$(pwd)":/workspace docker.io/asrlab/comp-org:pa0
```

```bash
cd src
make                    # builds executables 1, 2, 3 (static, RV64)
spike --isa=RV64GCV $RISCV/riscv64-unknown-linux-gnu/bin/pk 1 ../testcases/input/1_1.txt
make judge              # runs every public testcase through local-judge
```

## What I learned

- How the calling convention and register classes (integer `x` vs. floating-point `f`) meet at the
  C/asm boundary, and why a missing clobber causes silent corruption at `-O2`.
- Struct layout and pointer width on a 64-bit RISC-V ABI.
- Writing branch-heavy control flow directly in assembly without a compiler to schedule it.

## Lab series

| Lab | Repository | Topic |
|-----|------------|-------|
| 1 | **riscv-inline-asm-algorithms** (this repo) | RV64IF inline assembly |
| 2 | [rvv-mel-spectrogram](https://github.com/Adam010341/rvv-mel-spectrogram) | RISC-V Vector (RVV) intrinsics, FFT, DSP |
| 3 | [cache-aware-riscv-optimization](https://github.com/Adam010341/cache-aware-riscv-optimization) | Tree-PLRU cache simulator, cache-blocked transpose, RVV GEMM |

---

<sub>The test harness, I/O scaffolding and testcases were provided by the course staff.
The assembly implementations inside `asm volatile(...)` are my own work.</sub>
