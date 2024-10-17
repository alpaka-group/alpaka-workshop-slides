#!/usr/bin/env python

# Based on:
# https://github.com/openPMD/openPMD-api/blob/dev/examples/10_streaming_write.py

import json

from mpi4py import MPI

import numpy as np
import openpmd_api as io

# Runtime configuration. Here written as a JSON object and dumped via `json.dumps()`.
# https://openpmd-api.readthedocs.io/en/0.16.0/details/backendconfig.html
config = {"adios2": {"engine": {}, "dataset": {}}}
# pass-through for ADIOS2 engine parameters
# https://adios2.readthedocs.io/en/latest/engines/engines.html
config["adios2"]["engine"] = {"parameters": {"Threads": "4", "DataTransport": "WAN"}}
config["adios2"]["dataset"] = {"operators": [{"type": "bzip2"}]}

if __name__ == "__main__":
    # TODO: Initialize the Series with the MPI communicator
    comm = MPI.COMM_WORLD

    # create a series and specify some global metadata
    series = io.Series(
        "openPMD/simData_%T.bp5", io.Access_Type.create, json.dumps(config)
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
            pos.reset_dataset(io.Dataset(local_data.dtype, [length]))
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

        # TODO: What if a quantity is constant over the inspected mesh?
        # Or, maybe, we want to store constant properties of a particle species,
        # such as their mass, or a common position offset?
        # Storing an entire n-dimensional array with the same value repeated
        # seems silly. openPMD has a shorthand for this common case:
        # `Record_Component.make_constant(value)`.
        # Let's assume, we observe a field with a homogeneous temperature
        # across all cells. Try storing some temperature value here
        # and then inspect the resulting files using `bpls -al <file.bp5>`.
        # Tip: Since HDF5 and ADIOS2 generally have bad support for empty n-dimensional
        # datasets (i.e. extent 0 in at least one dimension), constant components
        # can also be used to store empty datasets, e.g. when all particles have
        # left the observed area. This can be done manually through `make_constant()`,
        # or more idiomatically with `make_empty()`.

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
