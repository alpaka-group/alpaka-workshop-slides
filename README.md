# Alpaka Training and Documentation Links

- **Training links for October 2024:**
  - [HandsOn Session Documentation](https://github.com/mehmetyusufoglu/alpaka-workshop-slides/blob/workshopOct2024/other-events/2024-October-Workshop/alpaka_merged_with_copy_buttons.md)
- **Git Repository:** [https://github.com/alpaka-group/alpaka](https://github.com/alpaka-group/alpaka)
- **Alpaka Documentation:**
  - Main Page: [https://alpaka.readthedocs.io/en/latest/index.html](https://alpaka.readthedocs.io/en/latest/index.html)
  - [Installation Guide](https://alpaka.readthedocs.io/en/latest/)
  - [Cheat Sheet](https://alpaka.readthedocs.io/en/latest/basic/cheatsheet.html)
  - [CMake Variables](https://alpaka.readthedocs.io/en/latest/advanced/cmake.html)
  - [API Docs](https://alpaka-group.github.io/alpaka/)
- **Webinar May 2024: Alpaka concepts and usage**
  - [Webinar Slides (pdf)](https://github.com/alpaka-group/alpaka-workshop-slides/blob/d40c44081c53041ce618205167c130c973c9b41e/slides-2024/UsingAlpakaForPlasmaPepscWebinar28May2024.pdf)
  

# Alpaka Workshop

Below is a rough WIP outline of the alpka workshop contents

## Why Alpaka?
- Support for **heterogeneous architectures** (e.g., CPUs, GPUs, FPGAs).
- Write once, run anywhere—Alpaka abstracts hardware specifics for parallel computing.

## How to Install

## Hands-On: Install and Run Example

- Compile and run an Alpaka example from the repository.
- Verify it runs on the available hardware (CPU, GPU, etc.).

## Parallelization Concepts
- Parallel Decomposition: Breaking the problem into independent parts.
- Chunking: Grouping tasks for parallel execution.
- Independent Work: Ensuring no dependencies between tasks.
- Example: Vector Addition - show decomposition of problem domain, and mapping to gpus

## GPU Concepts
- Grid: Our thread pool
- SMs: Streaming Multiprocessors.
- Threads and Blocks: Grouping of threads in blocks.
- Indexing: Indices of blocks and threads

## WorkDiv 
- Map the decomposed problem domain to blocks and threads
    - Difference on CPU and GPU
- Map thread and block indices to data

## Hands-On: Play with Indexing and Pitches
- Modify an example to experiment with indexing and printf
- Play with memory layout using pitches.

## Heat Equation Problem
- Introduction to solving the heat equation using Alpaka.
- Stencil Computation
    - Neighbor-based updates in the grid, often used in finite difference methods.
- Explain the workDiv

## Hands-On: Write the Kernel with mdspan
- Using uniformWork
- Use the mdspan library to manage multidimensional arrays.
- Implement a simple heat equation kernel using stencils.

## alpaka concepts
### Platform and Accelerator Concepts
### Buffers and Views: Managing memory across devices.
### Queues, Events, and Synchronization: Managing asynchronous operations.
### Launching Kernels

## Hands-On: Write the Main Code
- Use getValidWorkDiv to determine optimal grid configuration.

## Hands-On: Two Queues
- Implement parallel execution using two queues. Explore overlap between computation and data transfer.

## Hands-On: Explicit Chunking, Indexing and Shared Memory
- Implement explicit chunking of work and manage explicit indexing.
- Make custom work div based on chunking
- Add shared memory to improve performance.

## Timing Runs
- Measure the performance of your kernels and analyze the timing. with and w/o shared mem
