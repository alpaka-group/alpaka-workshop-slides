# Build and run

## Configure
cmake ..

## choose accelerator(s)
ccmake .

Hint for LUMI:
```bash
cmake .. \
    -Dalpaka_ACC_GPU_HIP_ENABLE=ON \
    -DCMAKE_HIP_ARCHITECTURES=gfx90a \
    -Dalpaka_USE_MDSPAN=FETCH \
    -DCMAKE_BUILD_TYPE=Release
```

## build
make -j

## execute
./heatEquation
