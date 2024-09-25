/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "helpers.hpp"

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
//! \param pitchCurr The pitch (or stride) in memory corresponding to the TDim grid in the accelerator's memory.
//!              This is used to calculate memory offsets when accessing elements in the current buffer.
//! \param pitchNext The pitch used to calculate memory offsets when accessing elements in the next buffer.
//! \param haloSize Size of halo required for our stencil in {Y, X} (above and to the left)
//! \param dx step in x
//! \param dy step in y
//! \param dt step in t
struct StencilKernel
{
    template<typename TAcc, typename TDim, typename TIdx>
    ALPAKA_FN_ACC auto operator()(
        TAcc const& acc,
        double const* const uCurrBuf,
        double* const uNextBuf,
        alpaka::Vec<TDim, TIdx> const& pitchCurr,
        alpaka::Vec<TDim, TIdx> const& pitchNext,
        alpaka::Vec<TDim, TIdx> const& haloSize,
        double const dx,
        double const dy,
        double const dt) const -> void
    {
        // Get indexes
        auto const gridThreadIdx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);

        // Each kernel executes one element
        double const rX = dt / (dx * dx);
        double const rY = dt / (dy * dy);

        // offset for halo, as we only want to go over core cells
        auto idx2D = gridThreadIdx + haloSize;
        auto const right = idx2D + alpaka::Vec<TDim, TIdx>{0, 1};
        auto const left = idx2D + alpaka::Vec<TDim, TIdx>{0, -1};
        auto const up = idx2D + alpaka::Vec<TDim, TIdx>{-1, 0};
        auto const down = idx2D + alpaka::Vec<TDim, TIdx>{1, 0};
        auto elem = getElementPtr(uNextBuf, idx2D, pitchNext);

        *elem = *getElementPtr(uCurrBuf, idx2D, pitchCurr) * (1.0 - 2.0 * rX - 2.0 * rY)
                + *getElementPtr(uCurrBuf, right, pitchCurr) * rX + *getElementPtr(uCurrBuf, left, pitchCurr) * rX
                + *getElementPtr(uCurrBuf, up, pitchCurr) * rY + *getElementPtr(uCurrBuf, down, pitchCurr) * rY;
    }
};
