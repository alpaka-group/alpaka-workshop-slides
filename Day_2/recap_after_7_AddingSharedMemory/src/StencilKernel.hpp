/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <alpaka/alpaka.hpp>

//! alpaka version of explicit finite-difference 2D heat equation solver
//!
//! \tparam T_SharedMemSize1D size of the shared memory box
//!
//! Solving equation u_t(x, t) = u_xx(x, t) + u_yy(y, t) using a simple explicit scheme with
//! forward difference in t and second-order central difference in x and y
//!
//! \param uCurrBuf Current buffer with grid values of u for each x, y pair and the current value of t:
//!                 u(x, y, t) | t = t_current
//! \param uNextBuf resulting grid values of u for each x, y pair and the next value of t:
//!              u(x, y, t) | t = t_current + dt
//! \param chunkSize The size of the chunk or tile that the user divides the problem into. This defines the size of the
//!                  workload handled by each thread block.
//! \param haloSize Size of halo required for our stencil in {Y, X} (above and to the left)
//! \param dx step in x
//! \param dy step in y
//! \param dt step in t
template<size_t T_SharedMemSize1D>
struct StencilKernel
{
    template<typename TAcc, typename TMdSpan, typename TDim, typename TIdx>
    ALPAKA_FN_ACC auto operator()(
        TAcc const& acc,
        TMdSpan uCurrBuf,
        TMdSpan uNextBuf,
        alpaka::Vec<TDim, TIdx> const& chunkSize,
        alpaka::Vec<TDim, TIdx> const& haloSize,
        double const dx,
        double const dy,
        double const dt) const -> void
    {
        auto& sdata = alpaka::declareSharedVar<double[T_SharedMemSize1D], __COUNTER__>(acc);
        auto smemSize2D = chunkSize + haloSize + haloSize;

        // use mdspan for easier set and access the 1D shared memory
        auto sdataMdSpan = std::experimental::mdspan<double, std::experimental::dextents<size_t, 2>>(
            sdata,
            smemSize2D[0],
            smemSize2D[1]);

        // Get indexes
        auto const gridBlockIdx = alpaka::getIdx<alpaka::Grid, alpaka::Blocks>(acc);
        auto const blockThreadIdx = alpaka::getIdx<alpaka::Block, alpaka::Threads>(acc);
        auto const blockStartThreadIdx = gridBlockIdx * chunkSize;

        // Each kernel executes one element
        double const rX = dt / (dx * dx);
        double const rY = dt / (dy * dy);

        auto const blockThreadExtent = alpaka::getWorkDiv<alpaka::Block, alpaka::Threads>(acc);

        // fill shared memory
        for(auto i = blockThreadIdx[0]; i < smemSize2D[0]; i += blockThreadExtent[0])
        {
            for(auto j = blockThreadIdx[1]; j < smemSize2D[1]; j += blockThreadExtent[1])
            {
                auto localIdx2D = alpaka::Vec(i, j);
                auto globalIdx = localIdx2D + blockStartThreadIdx;
                sdataMdSpan(localIdx2D[0], localIdx2D[1]) = uCurrBuf(globalIdx[0], globalIdx[1]);
            }
        }

        alpaka::syncBlockThreads(acc);

        // go over only core cells and update nextBuf
        for(auto i = blockThreadIdx[0]; i < chunkSize[0]; i += blockThreadExtent[0])
        {
            for(auto j = blockThreadIdx[1]; j < chunkSize[1]; j += blockThreadExtent[1])
            {
                // offset for halo, as we only want to go over core cells
                auto localIdx2D = alpaka::Vec(i, j) + haloSize;
                auto const globalIdx = localIdx2D + blockStartThreadIdx;

                uNextBuf(globalIdx[0], globalIdx[1])
                    = sdataMdSpan(localIdx2D[0], localIdx2D[1]) * (1.0 - 2.0 * rX - 2.0 * rY)
                      + sdataMdSpan(localIdx2D[0], localIdx2D[1] - 1) * rX
                      + sdataMdSpan(localIdx2D[0], localIdx2D[1] + 1) * rX
                      + sdataMdSpan(localIdx2D[0] - 1, localIdx2D[1]) * rY
                      + sdataMdSpan(localIdx2D[0] + 1, localIdx2D[1]) * rY;
            }
        }
    }
};
