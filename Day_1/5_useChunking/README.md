Solving the 2D Heat Equation in alpaka

Use chunking for the problem domain, and distribute work in chunks to blocks

Inside src, edit
- heatEquation2D.cpp 
    - Do the domain decomposition
    - Change the workDivision
- stencilKernel.hpp
    - Make sure each block handles a chunk

When the code is run after it is edited, it validates if the simulation results are correct