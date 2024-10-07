/* Copyright 2024 Benjamin Worpitz, Matthias Werner, Jakob Krude, Sergei
 * Bastrakov, Bernhard Manfred Gruber, Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#include "BoundaryKernel.hpp"
#include "InitializeBufferKernel.hpp"
#include "StencilKernel.hpp"
#include "analyticalSolution.hpp"

#ifdef PNGWRITER_ENABLED
#    include "writeImage.hpp"
#endif

#include <alpaka/alpaka.hpp>
#include <alpaka/example/ExecuteForEachAccTag.hpp>

#include <cmath>
#include <cstdint>
#include <iostream>

//! Each kernel computes the next step for one point.
//! Therefore the number of threads should be equal to numNodesX.
//! Every time step the kernel will be executed numNodesX-times
//! After every step the curr-buffer will be set to the calculated values
//! from the next-buffer.
//!
//! In standard projects, you typically do not execute the code with any
//! available accelerator. Instead, a single accelerator is selected once from
//! the active accelerators and the kernels are executed with the selected
//! accelerator only. If you use the example as the starting point for your
//! project, you can rename the example() function to main() and move the
//! accelerator tag to the function body.
template<typename TAccTag>
auto example(TAccTag const&) -> int
{
    // **************************************************************
    // * Define the alpaka platforms, devices, accelerators         *
    // **************************************************************

    // Set Dim and Idx type

    // Define the accelerator
    using Acc = alpaka::TagToAcc<TAccTag, Dim, Idx>;
    std::cout << "Using alpaka accelerator: " << alpaka::getAccName<Acc>() << std::endl;

    // Select specific devices

    // **************************************************************
    // * Define the simulation domain, time resolution, etc         *
    // **************************************************************
    // {Y, X}
    constexpr alpaka::Vec<Dim, Idx> numNodes{64, 64};
    // Size of halo required for our stencil in {Y, X} (above and to the left).
    constexpr alpaka::Vec<Dim, Idx> haloSize{1, 1};
    // Halo size must be multiplied by two to get the extents, as their are halo cells below and to the right as well
    constexpr alpaka::Vec<Dim, Idx> extent = numNodes + haloSize + haloSize;

    // Print the vectors, each has Dim number of elements
    std::cout << "The core [numNodes]:" << numNodes << std::endl;
    std::cout << "The halo [haloSize]:" << haloSize << std::endl;
    std::cout << "The full extent[nodes + 2*halosize]:" << extent << std::endl;

    constexpr uint32_t numTimeSteps = 4000;
    constexpr double tMax = 0.1;

    // x, y in [0, 1], t in [0, tMax]
    constexpr double dx = 1.0 / static_cast<double>(extent[1] - 1);
    constexpr double dy = 1.0 / static_cast<double>(extent[0] - 1);
    constexpr double dt = tMax / static_cast<double>(numTimeSteps);

    // Check the stability condition
    double r = 2 * dt / ((dx * dx * dy * dy) / (dx * dx + dy * dy));
    if(r > 1.)
    {
        std::cerr << "Stability condition check failed: dt/min(dx^2,dy^2) = " << r
                  << ", it is required to be <= 0.5\n";
        return EXIT_FAILURE;
    }

    // **************************************************************
    // * Define buffers                                             *
    // **************************************************************


    // Define host buffer

    // Define accelerator buffer

    // **************************************************************
    // * Execute Kernel to initialize the buffer                    *
    // *   1 - Create kernel object                                 *
    // *   2 - Create work division                                 *
    // *   3 - Create device queue                                  *
    // *   4 - Execute kernel                                       *
    // **************************************************************

    // Sets buffer to initial conditions

    // Define a workdiv for the given problem
    // Print the workdivision, it has 3 vector each having Dim number of elements
    // std::cout << "Workdivision:\n\t" << yourWorkDivisionName << std::endl;

    // Create queue
    alpaka::Queue<Acc, alpaka::NonBlocking> queue{devAcc};

    // Execute
    alpaka::exec<Acc>(queue, workDivExtent, initBufferKernel, uCurrBufAcc.data(), pitchCurrAcc, dx, dy);


    // **************************************************************
    // * Execute the simulation loop                                *
    // *   1 - Create stencil                                       *
    // *   2 - Create work division going over core cells           *
    // *   3 - Write execution loop with stencil, double buffering  *
    // *   4 - Aplly boundaries, IO, swap buffers                   *
    // **************************************************************


    // Simulate
    for(uint32_t step = 1; step <= numTimeSteps; ++step)
    {
        // Apply stencil

        // Apply boundaries
        applyBoundaries();

#ifdef PNGWRITER_ENABLED
        if((step - 1) % 100 == 0)
        {
            // **************************************************************
            // * Transfer data back to host for IO                          *
            // **************************************************************
            writeImage(step - 1, uBufHost);
        }
#endif
        // Swap
    }

    // **************************************************************
    // * Transfer data back to host to validate                     *
    // **************************************************************

    // Copy device -> host


    // Validate
    auto const [resultIsCorrect, maxError] = validateSolution(uBufHost, dx, dy, tMax);

    if(resultIsCorrect)
    {
        std::cout << "Execution results correct!" << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << "Execution results incorrect: Max error = " << maxError << " (the grid resolution may be too low)"
                  << std::endl;
        return EXIT_FAILURE;
    }
}

auto main() -> int
{
    // Execute the example once for each enabled accelerator.
    // If you would like to execute it for a single accelerator only you can use
    // the following code.
    //  \code{.cpp}
    //  auto tag = alpaka::TagCpuSerial{};
    //  return example(tag);
    //  \endcode
    //
    // valid tags:
    //   TagCpuSerial, TagGpuHipRt, TagGpuCudaRt, TagCpuOmp2Blocks,
    //   TagCpuTbbBlocks, TagCpuOmp2Threads, TagCpuSycl, TagCpuTbbBlocks,
    //   TagCpuThreads, TagFpgaSyclIntel, TagGenericSycl, TagGpuSyclIntel
    return alpaka::executeForEachAccTag([=](auto const& tag) { return example(tag); });
}
