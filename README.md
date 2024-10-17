# Links

- **Time Table for alpaka and OpenPMD Hackaton on 23-25 October 2024**
  - [TimeTable](https://events.hifis.net/event/1657/timetable/#all)

- **Presentations for alpaka Hackaton on 23-24 October 2024:**
  - [Introduction to Parallel Programming using alpaka](https://github.com/alpaka-group/alpaka-workshop-slides/blob/oct2024_workshop/presentations/UsingAlpakaWorkshopOctober2024.pdf)
  - [alpaka Features by Heat Equation Solution](https://github.com/alpaka-group/alpaka-workshop-slides/blob/oct2024_workshop/presentations/AlpakaFeaturesByHeatEquationOctober2024.pdf)
- **Repository for hands-on exercises and presentations**
  - [All documents of Workshop Alpaka Section, October 2024](https://github.com/alpaka-group/alpaka-workshop-slides/tree/oct2024_workshop)
  - [Preparation and Hands-On1: Connecting to LUMI and Installing Alpaka](https://github.com/alpaka-group/alpaka-workshop-slides/blob/oct2024_workshop/Day_1/alpaka_install_run_example.md)   
- **alpaka Git Repository:**
  - [https://github.com/alpaka-group/alpaka](https://github.com/alpaka-group/alpaka)

- **alpaka Documentation:**
  - Main Page: [https://alpaka.readthedocs.io/en/latest/index.html](https://alpaka.readthedocs.io/en/latest/index.html)
  - [Installation Guide](https://alpaka.readthedocs.io/en/latest/)
  - [Cheat Sheet](https://alpaka.readthedocs.io/en/latest/basic/cheatsheet.html)
  - [CMake Variables](https://alpaka.readthedocs.io/en/latest/advanced/cmake.html)
  - [API Docs](https://alpaka-group.github.io/alpaka/)
- **Webinar May 2024: alpaka concepts and usage**
  - [Webinar Slides (pdf)](https://github.com/alpaka-group/alpaka-workshop-slides/blob/d40c44081c53041ce618205167c130c973c9b41e/slides-2024/UsingAlpakaForPlasmaPepscWebinar28May2024.pdf)
  

# alpaka Workshop

The outline of the alpaka workshop

## Why alpaka?
- Support for **heterogeneous architectures** (e.g., CPUs, GPUs, FPGAs).
- Write once, run anywhere—alpaka abstracts hardware specifics for parallel computing.

## How to Install
- Install alpaka with correct cmake backend options
- Compile and run an alpaka example from the repository.
- Verify it runs on the available hardware (CPU, GPU, etc.).

## Hands-On 1: Install alpaka and Run An Example
- Install alpaka with correct cmake backend options and run an example

## Parallelization Concepts
- Parallel Decomposition: Breaking the problem into independent parts.
- Chunking: Grouping tasks for parallel execution.
- Independent Work: Ensuring no dependencies between tasks.
- Example: Vector Addition - show decomposition of problem domain, and mapping to GPUs.

## GPU Concepts
- Grid: Our thread pool.
- SMs: Streaming Multiprocessors.
- Threads and Blocks: Grouping of threads in blocks.
- Indexing: Indices of blocks and threads.

## WorkDiv
- Map the decomposed problem domain to blocks and threads.
    - Difference on CPU and GPU.
- Map thread and block indices to data.

## Hands-On 2: Indexing in Kernel: HelloIndex
- Modify an example to experiment with indexing and printf.

## Heat Equation Problem
- Introduction to solving the heat equation using alpaka.
- Memory Allocation and Passing to Kernel.
- Play with memory layout using pitches.

## Hands-On 3: Fill a buffer at the global memory in parallel

## Main Simulation Loop of Heat Equation
- Heat Equation Stencil Definition.

## Hands-On 4: Implement Stencil Kernel to Solve Heat Equation
- Implement a simple heat equation kernel using stencils.

## Programming Features and Data Structures of alpaka
- Accelerator, Device, Queue, Task.
- Buffers and Views: Managing memory across devices.
- alpaka mdspan.

## Optimizations

1. Use alpaka mdspan (Hands-On 5).
2. Domain Decomposition or "Chunking" (Hands-On 6).
3. Using async queues for performance increase (Hands-On 7).
4. Using shared memory for performance increase (Hands-On 8).

## Optimization Hands-Ons

### Hands-On 5: Write the Kernel with mdspan
- Use the mdspan to manage multidimensional arrays.
- Use the mdspan to pass multi-dim arrays to the kernel.

### Hands-On 6: Domain Decomposition (Chunking)
- Implement explicit chunking of work and manage explicit indexing.
- Make custom work division based on chunking.

### Hands-On 7: Two Queues
- Implement parallel execution using two queues. Explore overlap between computation and data transfer.

### Hands-On 8: Using Shared Memory
- Add shared memory to improve performance.

## Timing Runs
- Measure the performance of your kernels and analyze the timing with and without shared memory.
