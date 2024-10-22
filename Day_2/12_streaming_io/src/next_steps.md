## In-situ visualization of the data created by the Heat Equation example

1. We will again visualize the data created by the HeatEquation2D example, but this time not from disk.
    Instead we will run the HeatEquation2D example and visualization at the same time, streaming the openPMD data online.
2. We used file-based and group-based encoding so far.
    Group-based encoding can be inefficient when the data Series contains many Iterations due to backend-specific performance implications of having a large number of groups within a single file.
    For ADIOS2, a more efficient (but also slightly more restrictive) encoding is variable-based encoding that writes all data into the same place, but uses *variables*, i.e. datasets with changing content.
    Set `"variable_based"` iteration encoding in the JSON/TOML configuration, select the ADIOS2 backend, and inspect the content of the output file produced now.
3. Now open the Jupyter notebook `src/visualize.ipynb` provided as part of the current step and try running it.
    What do you see? Wait up to 10 seconds.
4. For an in-situ visualization, the data producer must run at the same time.
    Change the ADIOS2 engine to SST in the JSON/TOML config file.
    Open a Jupyter console (ideally side-by-side to the Notebook) and run the HeatEquation example.
    The consumer might have timed-out by now, in that case re-launch it.
5. Bonus: Inspect the SST options at [the ADIOS2 documentation](https://adios2.readthedocs.io/en/v2.10.1/engines/engines.html#sst-sustainable-staging-transport).
    Where would you apply those options?
    Try the effect of `QueueFullPolicy = "Discard"` or `RendezvousReaderCount = 0`.