/* Copyright 2024 Tapish Narwal
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <alpaka/alpaka.hpp>

template<typename T, typename U>
using const_match = std::conditional_t<std::is_const_v<T>, U const, U>;

//! Helper function to get a pointer to an element in a multidimensional buffer
//!
//! \tparam T type of the element
//! \tparam TDim dimension of the buffer
//! \tparam TIdx index type
//! \param ptr pointer to the buffer
//! \param idx index of the element
//! \param pitch pitch of the buffer
template<typename T, typename TDim, typename TIdx>
ALPAKA_FN_ACC T* getElementPtr(T* ptr, alpaka::Vec<TDim, TIdx> const& idx, alpaka::Vec<TDim, TIdx> const& pitch)
{
    // Solution assuming pitch[0] is a multiple of sizeof(T) or pitch[1].
    // Which is the usual case with standard build-in types like float, double, int, long int...
    return ptr + (idx[0] * pitch[0] + idx[1] * pitch[1])/sizeof(T);

    // If built-in data types are not used.
    // return reinterpret_cast<T*>(reinterpret_cast<const_match<T, std::byte>*>(ptr) + idx[0] * pitch[0] + idx[1] * pitch[1]);
}
