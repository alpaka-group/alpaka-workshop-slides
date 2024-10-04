Solving the 2D Heat Equation in alpaka

Fill the buffer with initial values for the heat equation simulation

Inside src, edit
- InitializeBufferKernel.hpp
- The 2D data is passed as a pointer and pitch-pair (pitch and datasize). Finding the data adress to be processed by the current thread (the 2D threadIndex is known) is the code to be implemented.

Only the kernel will be edited. When the code is run, it validates if the buffer is correctly filled
