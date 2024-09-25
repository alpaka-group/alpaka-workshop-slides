Solving the 2D Heat Equation in alpaka

Instead of dealing with pitches and pointers, use MDSpan

Inside src, edit
- heatEquation2D.cpp 
    - Remember to change the calls to initBufferKernel and applyBoundaries as well!
- stencilKernel.hpp

When the code is run after it is edited, it validates if the simulation results are correct