#!/usr/bin/env python

# Based on:
# https://github.com/openPMD/openPMD-api/blob/dev/examples/10_streaming_write.py

import json

# This is a workaround for LUMI, otherwise this will not work on non-GPU nodes.
# Would normally be specified in a job script or environment, but let's just
# hardcode this here for the purpose of this workshop :)
import os
os.environ["MPICH_GPU_SUPPORT_ENABLED"] = "0"

from mpi4py import MPI

import numpy as np
import openpmd_api as io

# TODO Let's try some configuration for parallel file I/O:
#
# First, try setting the ADIOS2 engine parameter `AggregatorRatio` to 1.
# Run the script in parallel with and without that parameter.
# Do you see a difference when looking into the `.bp5` folders?
# Hint: Check the aggregation options at
# https://adios2.readthedocs.io/en/latest/engines/engines.html#bp5
# It's generally a good idea to read through these options to get an idea
# of what you need to pay attention to on a parallel filesystem.
#
# Next, the JSON configuration below now contains a hint to HDF5 that the
# subfiling virtual file driver should be used.
# Run the example again using HDF5.
# Note that the subfiling VFD will not automatically merge the written files,
# this has to be done in a postprocessing step:
# `for i in openPMD/*.config; do h5fuse -f "$i"; done`.
#
# Bonus: What does the JSON engine do when executed in parallel?
# The output created by the JSON engine in parallel is intended more for debugging
# purposes, with limited support for further processing of the created files.
# Hint: For printing JSON in a readable way, use `jq`, it is installed by default
# on LUMI, no module needs to be loaded:
# `jq . < my_json_file.json`

# Runtime configuration. Here written as a JSON object and dumped via `json.dumps()`.
# https://openpmd-api.readthedocs.io/en/0.16.0/details/backendconfig.html
config = {"backend": "adios2", "adios2": {"engine": {}, "dataset": {}}, "hdf5": {}}
# pass-through for ADIOS2 engine parameters
# https://adios2.readthedocs.io/en/latest/engines/engines.html
config["adios2"]["engine"] = {"parameters": {"Threads": "4"}}
config["adios2"]["dataset"] = {"operators": [{"type": "bzip2"}]}
config["hdf5"] = {"vfd": {"type": "subfiling"}}

if __name__ == "__main__":
    comm = MPI.COMM_WORLD

    # create a series and specify some global metadata
    series = io.Series(
        "openPMD/simData_%T.%E", io.Access_Type.create, comm, json.dumps(config)
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
            pos.reset_dataset(io.Dataset(local_data.dtype, [comm.size * length]))
            pos[comm.rank * length : (comm.rank + 1) * length] = local_data

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
