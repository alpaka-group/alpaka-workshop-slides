
# Solving the 2D Heat Equation using Alpaka

The heat equation describes the distribution of heat in a given region over time. In this example, we will solve the 2D heat equation using the Alpaka parallel programming library. Alpaka allows us to abstract the accelerator hardware (CPU or GPU), providing a single code base for different platforms.

The 2D heat equation is given by:

\[
\frac{\partial u}{\partial t} = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} \right)
\]

We will discretize the equation using a forward difference in time and central difference in space, resulting in a finite difference method.

### Step 1: Initialize Grid and Parameters

We begin by defining our grid size, time steps, and other parameters. In this setup, the grid will have `NX` points in the x-direction and `NY` points in the y-direction. 

```cpp
const int NX = 64;
const int NY = 64;
const double alpha = 0.01;  // Diffusivity constant
const double dx = 1.0 / (NX - 1);  // Grid spacing in x
const double dy = 1.0 / (NY - 1);  // Grid spacing in y
const double dt = 0.01;     // Time step
const int num_steps = 1000; // Number of time steps
```

We also define arrays for storing the current and next temperature distributions.

### Step 2: Selecting an Alpaka Accelerator (HIP)

First, we select a HIP accelerator as Alpaka supports multiple backends. In this case, we use \`alpaka::AccGpuHipRt\` to choose a HIP GPU for acceleration.

```cpp
using Dim = alpaka::DimInt<2>;  // 2D problem
using Idx = std::size_t;  // Use size_t for indexing

// Define the accelerator: GPU HIP backend
using Acc = alpaka::AccGpuHipRt<Dim, Idx>;
auto const devAcc = alpaka::getDevByIdx<Acc>(0u);  // Select HIP device
using Queue = alpaka::Queue<Acc, alpaka::NonBlocking>;
Queue queue(devAcc);
```

This step ensures that the computation is executed on a HIP-enabled GPU.

### Step 3: Kernel for the 2D Heat Equation

The core of the solution involves applying the finite difference scheme to the grid points. In Alpaka, we define a kernel to execute this operation in parallel across the grid.

```cpp
class HeatEquationKernel
{
public:
    template<typename TAcc, typename TElem, typename TIdx>
    ALPAKA_FN_ACC auto operator()(
        TAcc const& acc,
        TElem const* uCurr,
        TElem* uNext,
        TIdx NX, TIdx NY,
        double alpha, double dx, double dy, double dt) const -> void
    {
        TIdx const x = alpaka::getIdx<alpaka::Grid, alpaka::Blocks>(acc)[0];
        TIdx const y = alpaka::getIdx<alpaka::Grid, alpaka::Blocks>(acc)[1];

        if (x > 0 && x < NX-1 && y > 0 && y < NY-1)
        {
            double u_xx = (uCurr[(x+1) * NY + y] - 2.0 * uCurr[x * NY + y] + uCurr[(x-1) * NY + y]) / (dx * dx);
            double u_yy = (uCurr[x * NY + (y+1)] - 2.0 * uCurr[x * NY + y] + uCurr[x * NY + (y-1)]) / (dy * dy);
            uNext[x * NY + y] = uCurr[x * NY + y] + alpha * dt * (u_xx + u_yy);
        }
    }
};
```

In the kernel, we compute the second derivatives using central difference approximations and update the temperature for each grid point.

### Step 4: Memory Management

In Alpaka, we need to allocate memory on the device and copy data between the host (CPU) and the device (GPU/CPU). We use \`alpaka::allocBuf\` to allocate buffers for \`uCurr\` and \`uNext\`, which store the temperature at the current and next time steps.

```cpp
auto uCurrBuf = alpaka::allocBuf<double, Idx>(devAcc, alpaka::Vec<Dim, Idx>{NX * NY});
auto uNextBuf = alpaka::allocBuf<double, Idx>(devAcc, alpaka::Vec<Dim, Idx>{NX * NY});

// Copy initial conditions from host to device
alpaka::memcpy(queue, uCurrBuf, uHost);
```

### Step 5: Work Division (Updated)

The work division is set similar to the template example, which splits the problem into chunks and defines the thread and block sizes.

```cpp
constexpr alpaka::Vec<Dim, Idx> elemPerThread{1, 1};
constexpr Idx xSize = 16u;
constexpr Idx ySize = 16u;
constexpr Idx halo = 2u;
constexpr alpaka::Vec<Dim, Idx> chunkSize{ySize, xSize};
constexpr auto sharedMemSize = (ySize + halo) * (xSize + halo);

constexpr alpaka::Vec<Dim, Idx> numChunks{
    alpaka::core::divCeil(numNodes[0], chunkSize[0]),
    alpaka::core::divCeil(numNodes[1], chunkSize[1]),
};

alpaka::WorkDivMembers<Dim, Idx> workDiv{
    numChunks,
    chunkSize,
    elemPerThread
};
```

### Step 6: Executing the Kernel

Now we can execute the kernel for a specified number of time steps. After each step, we swap the buffers (\`uCurr\` and \`uNext\`) to update the grid for the next iteration.

```cpp
for (int step = 0; step < num_steps; ++step)
{
    // Execute the kernel
    alpaka::exec<Acc>(
        queue,
        workDiv,
        heatEquationKernel,
        uCurrBuf.data(),
        uNextBuf.data(),
        NX, NY,
        alpha, dx, dy, dt
    );

    // Swap buffers for the next iteration
    std::swap(uCurrBuf, uNextBuf);
}
```

### Step 7: Copy Result Back to Host

After all time steps have been completed, the final temperature distribution is copied back to the host for visualization or further processing.

```cpp
alpaka::memcpy(queue, uHost, uCurrBuf);
alpaka::wait(queue);

// Print the result or save it to a file
```

### Conclusion

Using Alpaka, we can easily parallelize the solution of the 2D heat equation across different accelerators, such as GPUs or multi-core CPUs, while maintaining the same codebase. Alpaka’s abstraction makes it possible to write hardware-independent code with minimal changes required for execution on different devices.

This example demonstrates the use of Alpaka to:
- Select and configure a HIP accelerator.
- Define kernels for numerical operations.
- Manage memory and data movement between host and device.
- Set up work division and execute kernels in parallel.
