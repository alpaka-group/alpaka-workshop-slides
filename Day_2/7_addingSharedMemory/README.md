Solving the 2D Heat Equation in alpaka

Use shared memory to speed up computation on GPU accelerators

Inside src, edit
- heatEquation2D.cpp
    - Define the size of the shared memory
    - Pass shared memory size to the kernel
- StencilKernel.hpp
    - Fill shared memory in parallel
    - Use shared memory in stencil

When the code is run after it is edited, it validates if the simulation results are correct
