Solving the 2D Heat Equation in alpaka

Write the kernel for the heat equation stencil

Inside src, edit
- StencilKernel.hpp
- Elementwise multiplication and sum of stencil and the corresponding 5 points in data (namely Frobenius Dot Product) is the main part of the code that needs to be implemented.

When the code is run, it simulates the heat equation and validates if the stencil is correct
