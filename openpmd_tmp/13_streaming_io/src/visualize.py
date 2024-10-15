#!/usr/bin/env python

import IPython.display as display
import matplotlib.pylab as plt
import numpy as np
import openpmd_api as opmd

import sys


def get_filename():
    if len(sys.argv) != 2:
        print("Usage: {} <file-pattern>".format(sys.argv[0]))
        sys.exit(1)
    else:
        return sys.argv[1]


def print_unitdimension(ud):
    units = {
        opmd.Unit_Dimension.L: "m",
        opmd.Unit_Dimension.M: "kg",
        opmd.Unit_Dimension.T: "s",
        opmd.Unit_Dimension.I: "A",
        opmd.Unit_Dimension.theta: "K",
        opmd.Unit_Dimension.N: "mol",
        opmd.Unit_Dimension.J: "cd",
    }
    return " * ".join(
        (
            units[opmd.Unit_Dimension(idx)]
            if int(val) == 1
            else "{}^{}".format(units[opmd.Unit_Dimension(idx)], int(val))
        )
        for idx, val in enumerate(ud)
        if val != 0
    )


def show_iteration(iteration, figure_settings):
    # Pass wait=True to wait until new ouput before clearing; this prevents flickering
    display.clear_output(wait=True)
    plt.clf()

    heat = iteration.meshes["heat"]
    data = heat[:, :]
    iteration.close()

    # contourf prints the axes in reverse order, counteract this
    data = heat.unit_SI * data.T

    # Stick with the color map from the first image by fixing vmin and vmax
    if figure_settings.vmin is None or figure_settings.vmax is None:
        figure_settings.vmin = data.min()
        figure_settings.vmax = data.max()

    plt.title(
        "Heat at step {} [{}]".format(
            str(iteration.iteration_index), print_unitdimension(heat.unit_dimension)
        )
    )
    plt.xlabel(heat.axis_labels[0])
    plt.ylabel(heat.axis_labels[1])

    def get_coordinates(dim):
        offset = heat.grid_global_offset[dim] * heat.grid_unit_SI
        spacing = heat.grid_spacing[dim]
        spacing *= heat.grid_unit_SI
        ticks = np.fromiter(
            (offset + spacing * i for i in range(heat.shape[dim])), dtype=type(offset)
        )
        return ticks

    def get_offset(dim):
        return heat.grid_global_offset[dim] * heat.grid_unit_SI

    def get_extent(dim):
        spacing = heat.grid_spacing[dim] * heat.grid_unit_SI
        extent = heat.shape[dim] * spacing
        return extent

    plt.imshow(
        data,
        cmap=plt.cm.inferno,
        vmin=figure_settings.vmin,
        vmax=figure_settings.vmax,
        extent=(get_offset(0), get_extent(0), get_offset(1), get_extent(1)),
    )
    figure_settings.colorbar = plt.colorbar()

    figure_settings.display_id.update(plt.gcf())
    # plt.pause(0.1)


class FigureSettings:
    def __init__(self, display_id):
        self.colorbar = None
        self.vmin = None
        self.vmax = None
        self.display_id = display_id


def main(filename, config=""):
    s = opmd.Series(
        filename,
        opmd.Access.read_linear,
        opmd.merge_json("defer_iteration_parsing = true", config),
    )

    # interactive off
    plt.ioff()
    figure_settings = FigureSettings(display.display(display_id=True))
    # interactive on
    plt.ion()

    for iteration in s.read_iterations():
        show_iteration(iteration, figure_settings)
    s.close()


if __name__ == "__main__":
    main(get_filename())
