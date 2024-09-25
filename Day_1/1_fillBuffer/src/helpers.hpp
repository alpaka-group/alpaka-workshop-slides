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
    // **************************************************************
    // * Return the pointer to element in ND memory                 *
    // **************************************************************
}
