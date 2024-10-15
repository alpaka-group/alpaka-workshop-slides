## In-situ visualization of the data created by the Heat Equation example

1. The source directory now contains a Python file `visualize.py`.
    It can be loaded as a module (e.g. `import 12_json_toml_config.src.visualize as vis`) and provides an entry function `def main(filepath, config="")`.
    In order to run a visualization on a remote system, we use a Jupyter Notebook, which Lumi provides at `https://lumi.csc.fi`.
    Select partition `small-g`, at least 2 CPU cores, 4GB of memory.
    Select advanced settings and specify `/project/project_465001310/workshop_software/env.sh` as init script.
    Create a new Python Notebook and try visualizing one of the data Series written so far.
1. We used file-based and group-based encoding so far.
    Group-based encoding can be inefficient when the data Series contains many Iterations due to backend-specific performance implications of having a large number of groups within a single file.
    For ADIOS2, a more efficient (but also slightly more restrictive) encoding is variable-based encoding that writes all data into the same place, but uses *variables*, i.e. datasets with changing content.
    Set `"variable_based"` iteration encoding in the JSON/TOML configuration, select the ADIOS2 backend, and inspect the content of the output file produced now.