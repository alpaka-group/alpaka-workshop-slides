#!/usr/bin/env python

# Based on:
# https://github.com/openPMD/openPMD-api/blob/dev/examples/10_streaming_write.py

import json

from mpi4py import MPI

import numpy as np
import openpmd_api as io

# Runtime configuration. Here written as a JSON object and dumped via `json.dumps()`.
# https://openpmd-api.readthedocs.io/en/0.16.0/details/backendconfig.html
config = {"backend": "adios2", "adios2": {"engine": {}, "dataset": {}}}
# pass-through for ADIOS2 engine parameters
# https://adios2.readthedocs.io/en/latest/engines/engines.html
config["adios2"]["engine"] = {"parameters": {"Threads": "4"}}
config["adios2"]["dataset"] = {"operators": [{"type": "bzip2"}]}

if __name__ == "__main__":
    # TODO: Initialize the Series with the MPI communicator.
    # Note that for execution, you need to set `export MPICH_GPU_SUPPORT_ENABLED=0`,
    # otherwise mpi4py will try looking for GPUs.
    comm = MPI.COMM_WORLD

    # create a series and specify some global metadata
    series = io.Series(
        "openPMD/simData_%T.%E", io.Access_Type.create, json.dumps(config)
    )
    series.set_author("Franz Poeschel <f.poeschel@hzdr.de>")
    series.set_software("openPMD-api-python-examples")

    # now, write a number of iterations (or: snapshots, time steps)
    for i in range(10):
        # Direct access to iterations is possible via `series.iterations`.
        # For streaming support, `series.write_iterations()` needs to be used
        # instead of `series.iterations`.
        # `Series.write_iterations()` and `Series.read_iterations()` are
        # intentionally restricted APIs that ensure a workflow which also works
        # in streaming setups, e.g. an iteration cannot be opened again once
        # it has been closed.
        iteration = series.write_iterations()[i]

        #######################
        # write electron data #
        #######################

        electronPositions = iteration.particles["e"]["position"]

        # openPMD attribute
        # (this one would also be set automatically for positions)
        electronPositions.unit_dimension = {io.Unit_Dimension.L: 1.0}
        # custom attribute
        electronPositions.set_attribute("comment", "I'm a comment")

        length = 10
        local_data = np.arange(i * length, (i + 1) * length, dtype=np.dtype("double"))
        for dim in ["x", "y", "z"]:
            pos = electronPositions[dim]
            # TODO: Each MPI rank contributes 10 electrons.
            # The below dataset definition specifies the global extent of the dataset
            # across all MPI ranks. The global size depends on the number
            # of ranks that contribute, modify the dataset definition accordingly.
            # Hint: Particle data is always a one-dimensional list in openPMD.
            # Use `comm.size` to find the number of MPI ranks.
            pos.reset_dataset(io.Dataset(local_data.dtype, [length]))
            # TODO: So far, we were able to use the below shorthand for writing from
            # a slice to the entire openPMD component.
            # Since the local rank now only contributes one block within the global
            # dataset, we need to be explicit now on where the block will end up.
            # Hint: The Python syntax for one-dimensional slices is:
            # `pos[start:end]` where the start is inclusive and the end exclusive.
            # Use `comm.rank` to find the local rank index.
            pos[()] = local_data

        # optionally: flush now to clear buffers
        # alternatively: `series.flush()` or `iteration.iteration_flush()`
        iteration.iteration_flush()

        ###############################
        # write some temperature data #
        ###############################

        temperature = iteration.meshes["temperature"]
        temperature.unit_dimension = {io.Unit_Dimension.theta: 1.0}
        temperature.axis_labels = ["x", "y"]
        temperature.grid_spacing = [1.0, 1.0]
        # temperature has no x,y,z components, so skip the last layer:
        temperature_dataset = temperature
        # let's say we are in a 3x3 mesh
        temperature_dataset.reset_dataset(io.Dataset(np.dtype("double"), [3, 3]))
        # temperature is constant
        temperature_dataset.make_constant(273.15)

        # After closing the iteration, the readers can see the iteration.
        # It can no longer be modified.
        # If not closing an iteration explicitly, it will be implicitly closed
        # upon creating the next iteration.
        iteration.close()

    # The files in 'series' are still open until the object is destroyed, on
    # which it cleanly flushes and closes all open file handles.
    # When running out of scope on return, the 'Series' destructor is called.
    # Alternatively, one can call `series.close()` to the same effect as
    # calling the destructor, including the release of file handles.
    series.close()
