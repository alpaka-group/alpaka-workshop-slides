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
  



# The outline of the alpaka workshop

# Section - I

## 1 Introduction: What is alpaka, where it is used?
- Support for **heterogeneous architectures** (e.g., CPUs, GPUs, FPGAs).
- Write once, run anywhere—alpaka abstracts hardware specifics for parallel computing.

## 2 Hands on 1: Installing alpaka and running an example (LUMI)
- Install alpaka with correct cmake backend options
- Compile and run an alpaka example from the repository.
- Verify it runs on the available hardware (CPU, GPU, etc.).

## 3 Parallel programming concepts and portable parallelization by alpaka
- Grid Structure and WorkDivision
- Data Parallelism
- Indexing

## 4 Hands on 2: HelloIndex kernel which prints indexes

# Section - II

## 1 Memory management for 1D and 2D data
- Memory Allocation, Padding and Pitch

## 2 Filling buffers in parallel
- Use indexing to match thread to data

## 3 Hands on 3: Kernel to fill initial conditions of heat equation

## 4 Heat Equation
- Double accelerator-buffers method to solve Heat Equation
## 5 Preparing stencil kernel
- Difference Equation as a stencil operation

## 6 Hands on 4: Heat Equation stencil kernel

## 7 alpaka programming features and data-structures
- Accelerator, Device, Queue, Task
- Buffers and Views: Managing memory across devices
- alpaka mdspan
- Setting work division manually

## 8 Usability and optimization
- Hands on 5: Using alpaka mdspan for easier indexing 
- Hands on 6: Domain Decomposition, chunking or tiling
- Hands on 7: Using multiple Queues to increase performance. Explore overlap between computation and data transfer
- Hands on 8: Using Shared Memory for chunksExplore overlap between computation and data transfer

## Timing Runs
- Measure the performance of your kernels and analyze the timing with and without shared memory
