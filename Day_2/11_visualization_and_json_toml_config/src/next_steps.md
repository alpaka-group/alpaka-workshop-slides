## Visualization of the data created by the Heat Equation example

1. The source directory now contains a Python file `visualize.py`.
    It can be loaded as a module (e.g. `import 12_json_toml_config.src.visualize as vis`) and provides an entry function `def main(filepath, config="")`.
2.  In order to run a visualization on a remote system, we use a Jupyter Notebook, which Lumi provides at `https://lumi.csc.fi` ("Jupyter" App).
    Ideally use Chrome (Firefox had flickering problems with the visualization when I tested it), select partition `small-g`, at least 2 CPU cores, 4GB of memory.
    Select advanced settings and specify `/project/project_465001310/workshop_software/env.sh` as init script.
3.  Create a new Python Notebook and try visualizing one of the data Series written so far.

##  Runtime backend configuration via JSON/TOML, e.g. for compression options

See the inline comments in `openPMDOutput.hpp`.