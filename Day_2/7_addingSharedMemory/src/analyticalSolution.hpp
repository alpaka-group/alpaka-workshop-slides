/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <alpaka/alpaka.hpp>

#include <cmath>

//! Exact solution to the test problem
//! u_t(x, y, t) = u_xx(x, t) + u_yy(y, t), x in [0, 1], y in [0, 1], t in [0, T]
//!
//! \param x value of x
//! \param x value of y
//! \param t value of t
template<typename TAcc>
ALPAKA_FN_ACC auto analyticalSolution(TAcc const& acc, double const x, double const y, double const t) -> double
{
    constexpr double pi = alpaka::math::constants::pi;
    return alpaka::math::exp(acc, -pi * pi * t) * (alpaka::math::sin(acc, pi * x) + alpaka::math::sin(acc, pi * y));
}

auto analyticalSolution(double const x, double const y, double const t) -> double
{
    constexpr double pi = alpaka::math::constants::pi;
    return std::exp(-pi * pi * t) * (std::sin(pi * x) + std::sin(pi * y));
}

//! Valdidate calculated solution in the buffer to the analytical solution at t=tMax
//!
//! \param buffer buffer holding the solution at t
//! \param dx
//! \param dy
//! \param t
template<typename T_Buffer>
auto validateSolution(T_Buffer const& buffer, double const dx, double const dy, double const t)
    -> std::pair<bool, double>
{
    auto extents = alpaka::getExtents(buffer);
    // Calculate error
    double maxError = 0.0;
    for(uint32_t j = 1; j < extents[0] - 1; ++j)
    {
        for(uint32_t i = 1; i < extents[1] - 1; ++i)
        {
            auto const error = std::abs(buffer.data()[j * extents[1] + i] - analyticalSolution(i * dx, j * dy, t));
            maxError = std::max(maxError, error);
        }
    }

    constexpr double errorThreshold = 1e-4;
    return std::make_pair(maxError < errorThreshold, maxError);
}
