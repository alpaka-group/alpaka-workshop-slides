Below is a rough WIP outline of the alpka workshop contents 

# Alpaka Workshop

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
