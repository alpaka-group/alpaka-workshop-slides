/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "analyticalSolution.hpp"

#include <alpaka/alpaka.hpp>

#include <cstdio>

//! alpaka version of explicit finite-difference 2D heat equation solver
//!
//! Solving equation u_t(x, t) = u_xx(x, t) + u_yy(y, t) using a simple explicit scheme with
//! forward difference in t and second-order central difference in x and y
//!
//! \param bufData Current buffer data with grid values of u for each x, y pair and the current value of t:
//!                 u(x, y, t) | t = t_current
//! \param dx
//! \param dy
struct InitializeBufferKernel
{
    template<typename TAcc, typename TMdSpan>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc, TMdSpan bufData, double dx, double dy) const -> void
    {
        // Get indexes
        auto const gridThreadIdx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);

        bufData(gridThreadIdx[0], gridThreadIdx[1])
            = analyticalSolution(acc, gridThreadIdx[1] * dx, gridThreadIdx[0] * dy, 0.0);
    }
};
