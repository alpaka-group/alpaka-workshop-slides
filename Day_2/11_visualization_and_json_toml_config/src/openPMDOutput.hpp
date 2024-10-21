#pragma once

#include <alpaka/alpaka.hpp>
#include <openPMD/openPMD.hpp>

#ifdef OPENPMD_ENABLED

struct OpenPMDOutput
{
private:
    openPMD::Series m_series;

    template<typename Vec>
    static auto asOpenPMDExtent(Vec const& vec) -> openPMD::Extent
    {
        return openPMD::Extent{vec.begin(), vec.end()};
    }

public:
    void init()
    {
        /*
         * First follow the steps in `next_steps.md` for visualizing the data written so far.
         * Then, follow the steps below.
         *
         * TODO: The source folder contains two config files openpmd_config.json and openpmd_config.toml. Their content
         * is identical, openPMD accepts both TOML-formatted and JSON-formatted configuration. TOML is more legible as
         * a configuration format (and supports comments), while JSON is more widely known. You may continue with
         * either JSON or TOML as you prefer. In order to apply this configuration file, the following constructor call
         * must be modified:
         *
         * 1. Since the backend is now specified at runtime, the filename extension should no longer be hardcoded.
         *    Replace the extension with `%E` in order to let openPMD choose an extension. (For read workflows: `%E`
         *    will inspect the filesystem for existing files that might match the pattern.)
         * 2. The JSON/TOML configs specify group-based iteration encoding which will write only one file. The
         *    expansion pattern in the file name is no longer needed, so we can remove it.
         * 3. The path to the JSON/TOML config should be specified. Note that JSON/TOML configuration can also be
         *    passed inline, so paths need to be distinguished by beginning with a `@`. A good idea is to read the
         *    configuration file from the run directory; for this, specify just the filename and then copy/symlink the
         *    configuration into e.g. your build folder.
         *
         * You may also consult the Doxygen documentation for the `Series` constructor at
         * https://openpmd-api.readthedocs.io/en/0.16.0/_static/doxyhtml/index.html.
         * Use this documentation page for the syntax of JSON/TOML configuration:
         * https://openpmd-api.readthedocs.io/en/0.16.0/details/backendconfig.html
         *
         * Bonus: Try writing an HDF5 file instead of ADIOS2!
         */
        m_series = openPMD::Series("openpmd/heat_%06T.bp5", openPMD::Access::CREATE);
        m_series.setMeshesPath("images");
        m_series.setAuthor("Franz Poeschel");
        m_series.setSoftware("Alpaka HeatEquation2D Example");
    }

    template<typename HostBuffer, typename AccBuffer, typename DumpQueue>
    void writeIteration(
        openPMD::Iteration::IterationIndex_t step,
        HostBuffer& hostBuffer,
        AccBuffer& accBuffer,
        DumpQueue& dumpQueue)
    {
        using value_t = alpaka::Elem<HostBuffer>;

        openPMD::Iteration current_iteration = m_series.writeIterations()[step];
        openPMD::Mesh image = current_iteration.meshes["heat"];

        image.setAxisLabels({"x", "y"});
        // This would be relevant if we were writing a sub-mesh within a larger
        // global simulation. We are not, so keep this as zero.
        image.setGridGlobalOffset({0., 0.});
        // The mesh is given in terms of micrometers
        image.setGridUnitSI(0.001);
        // Let's say five micrometers.
        image.setGridSpacing(std::vector<double>{5, 5});
        // Each pixel represents the center of its grid cell
        image.setPosition(std::vector<double>{0.5, 0.5});
        // The SI unit of the field is Kelvin
        // How would you write Joule?
        image.setUnitDimension({{openPMD::UnitDimension::theta, 1.0}});
        // The simulation may use custom scaled units, e.g.: 0.1 Kelvin
        image.setUnitSI(0.1);

        auto logical_extents = alpaka::getExtents(accBuffer);

        image.resetDataset({openPMD::determineDatatype<value_t>(), asOpenPMDExtent(logical_extents)});

        alpaka::memcpy(dumpQueue, hostBuffer, accBuffer);
        alpaka::wait(dumpQueue);

        image.storeChunkRaw(hostBuffer.data(), {0, 0}, asOpenPMDExtent(logical_extents));

        current_iteration.close();
    }

    void close()
    {
        m_series.close();
    }
};

#else

struct OpenPMDOutput
{
    void init()
    {
    }

    template<typename... Args>
    void writeIteration(Args&&...)
    {
    }

    void close()
    {
    }
};
#endif
