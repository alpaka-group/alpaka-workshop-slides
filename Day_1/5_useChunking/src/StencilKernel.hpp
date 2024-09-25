/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <alpaka/alpaka.hpp>

//! alpaka version of explicit finite-difference 2D heat equation solver
//!
//! Solving equation u_t(x, t) = u_xx(x, t) + u_yy(y, t) using a simple explicit scheme with
//! forward difference in t and second-order central difference in x and y
//!
//! \param uCurrBuf Current buffer with grid values of u for each x, y pair and the current value of t:
//!                 u(x, y, t) | t = t_current
//! \param uNextBuf resulting grid values of u for each x, y pair and the next value of t:
//!              u(x, y, t) | t = t_current + dt
//! \param haloSize Size of halo required for our stencil in {Y, X} (above and to the left)
//! \param dx step in x
//! \param dy step in y
//! \param dt step in t
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
        using Dim = alpaka::Dim<TAcc>;
        using Idx = alpaka::Idx<TAcc>;

        // **************************************************************
        // * Change to handle one chunk per block                       *
        // **************************************************************

        // Get indexes
        auto const gridThreadIdx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);

        // Each kernel executes one element
        double const rX = dt / (dx * dx);
        double const rY = dt / (dy * dy);

        // offset for halo, as we only want to go over core cells
        auto idx2D = gridThreadIdx + haloSize;

        uNextBuf(idx2D[0], idx2D[1]) = uCurrBuf(idx2D[0], idx2D[1]) * (1.0 - 2.0 * rX - 2.0 * rY)
                                       + uCurrBuf(idx2D[0], idx2D[1] + 1) * rX + uCurrBuf(idx2D[0], idx2D[1] - 1) * rX
                                       + uCurrBuf(idx2D[0] + 1, idx2D[1]) * rY + uCurrBuf(idx2D[0] - 1, idx2D[1]) * rY;
    }
};
