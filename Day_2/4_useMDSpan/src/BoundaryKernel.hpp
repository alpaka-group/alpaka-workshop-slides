/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "analyticalSolution.hpp"

#include <alpaka/alpaka.hpp>

//! alpaka version of explicit finite-difference 1d heat equation solver
//!
//! Applies boundary conditions
//! forward difference in t and second-order central difference in x
//!
//! \param uBuf grid values of u for each x, y and the current value of t:
//!                 u(x, y, t)  | t = t_current
//! \param step simulation timestep
//! \param dx step in x
//! \param dy step in y
//! \param dt step in t
struct BoundaryKernel
{
    template<typename TAcc, typename TMdSpan>
    ALPAKA_FN_ACC auto operator()(
        TAcc const& acc,
        TMdSpan uBuf,
        uint32_t step,
        double const dx,
        double const dy,
        double const dt) const -> void
    {
        // Get extents(dimensions)
        auto const gridThreadExtent = alpaka::getWorkDiv<alpaka::Grid, alpaka::Threads>(acc);

        // Get indexes
        auto const globalIdx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);

        if(globalIdx[0] == 0 || globalIdx[0] == gridThreadExtent[0] - 1 || globalIdx[1] == 0
           || globalIdx[1] == gridThreadExtent[1] - 1)
        {
            uBuf(globalIdx[0], globalIdx[1])
                = analyticalSolution(acc, globalIdx[1] * dx, globalIdx[0] * dy, step * dt);
        }
    }
};

template<typename TAcc, typename TWorkDiv, typename TQueue, typename... TArgs>
auto applyBoundaries(TWorkDiv const& workDiv, TQueue& queue, TArgs&&... args) -> void
{
    static BoundaryKernel boundaryKernel{};

    alpaka::exec<TAcc>(queue, workDiv, boundaryKernel, args...);
}
