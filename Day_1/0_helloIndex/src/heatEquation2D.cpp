/* Copyright 2024 Benjamin Worpitz, Matthias Werner, Jakob Krude, Sergei
 * Bastrakov, Bernhard Manfred Gruber, Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#include "InitializeBufferKernel.hpp"

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
    // Set Dim and Idx type
    using Dim = alpaka::DimInt<2u>;
    using Idx = uint32_t;

    // Define the accelerator
    using Acc = alpaka::TagToAcc<TAccTag, Dim, Idx>;
    std::cout << "Using alpaka accelerator: " << alpaka::getAccName<Acc>() << std::endl;

    // Select specific devices
    auto const platformHost = alpaka::PlatformCpu{};
    auto const devHost = alpaka::getDevByIdx(platformHost, 0);
    auto const platformAcc = alpaka::Platform<Acc>{};
    // get suitable device for this Acc
    auto const devAcc = alpaka::getDevByIdx(platformAcc, 0);

    constexpr alpaka::Vec<Dim, Idx> numNodes{64, 64};
    // Size of halo required for our stencil in {Y, X} (above and to the left).
    constexpr alpaka::Vec<Dim, Idx> haloSize{1, 1};
    // Halo size must be multiplied by two to get the extents, as their are halo cells below and to the right as well
    constexpr alpaka::Vec<Dim, Idx> extent = numNodes + haloSize + haloSize;

    // x, y in [0, 1], t in [0, tMax]
    constexpr double dx = 1.0 / static_cast<double>(extent[1] - 1);
    constexpr double dy = 1.0 / static_cast<double>(extent[0] - 1);

    // Initialize host-buffer
    auto uBufHost = alpaka::allocBuf<double, Idx>(devHost, extent);

    // Accelerator buffer
    auto uBufAcc = alpaka::allocBuf<double, Idx>(devAcc, extent);

    // Set buffer to initial conditions
    InitializeBufferKernel initBufferKernel;
    // Define a workdiv for the given problem
    constexpr alpaka::Vec<Dim, Idx> elemPerThread{1, 1};

    alpaka::KernelCfg<Acc> const kernelCfg = {extent, elemPerThread};

    auto const pitchCurrAcc{alpaka::getPitchesInBytes(uBufAcc)};

    auto workDivExtent
        = alpaka::getValidWorkDiv(kernelCfg, devAcc, initBufferKernel, uBufAcc.data(), pitchCurrAcc, dx, dy);

    // Create queue
    alpaka::Queue<Acc, alpaka::NonBlocking> queue{devAcc};

    alpaka::exec<Acc>(queue, workDivExtent, initBufferKernel, uBufAcc.data(), pitchCurrAcc, dx, dy);

    // Copy device -> host
    alpaka::memcpy(queue, uBufHost, uBufAcc);
    alpaka::wait(queue);

    std::cout << std::endl << alpaka::getAccName<Acc>() << " Execution finished!" << std::endl;
    return EXIT_SUCCESS;
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
