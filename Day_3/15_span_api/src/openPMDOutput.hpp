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
        m_series = openPMD::Series("openpmd/heat_%T.%E", openPMD::Access::CREATE, "@./openpmd_config.toml");
        m_series.setMeshesPath("images");
        m_series.setAuthor("Franz Poeschel");
        m_series.setSoftware("Alpaka HeatEquation2D Example");
    }

    template<typename DevHost, typename AccBuffer, typename DumpQueue>
    void writeIteration(
        openPMD::Iteration::IterationIndex_t step,
        DevHost& devHost,
        AccBuffer& accBuffer,
        DumpQueue& dumpQueue)
    {
        using value_t = alpaka::Elem<AccBuffer>;

        openPMD::Iteration current_iteration = m_series.writeIterations()[step];
        // Time is given in terms of nanoseconds
        current_iteration.setTimeUnitSI(10e-9);
        // Let's say, homogeneously 50 nanoseconds per step
        current_iteration.setTime(step * 50.0);

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

        openPMD::Dataset dataset_definition{openPMD::determineDatatype<value_t>(), asOpenPMDExtent(logical_extents)};
        image.resetDataset(dataset_definition);

        auto openPMDBuffer = image.storeChunk<value_t>({0, 0}, asOpenPMDExtent(logical_extents));
        /*
         * TODO: Bonus task.
         * Until now, we downloaded the buffer from GPU (implicitly removing the padding) and then
         * wrote that downloaded data into openPMD.
         * Since some backends internally buffer their data before output
         * (for improving IO performance by runnning few big operations instead of many small ones),
         * this is an uneeded duplicate copy.
         * This is a realistic use case, since data will in reality often be in a custom format
         * tailored toward the algorithms used e.g. by the simulation, meaning that
         * a data preparation of sorts will be required before output.
         * With the call above, we can ask openPMD to give us direct access to the internal staging
         * buffers.
         * We can now download the GPU data directly into that buffer instead of the hostBuffer
         * allocated previously.
         *
         * Task:
         *
         * 1. Use `alpaka::createView()` to make the openPMDBuffer compatible with Alpaka.
         * 2. Use `alpaka::memcpy()` and `alpaka::wait()` to download data directly into that view.
         *
         * Hints:
         *
         * The openPMDBuffer has this type:
         * https://openpmd-api.readthedocs.io/en/0.16.0/_static/doxyhtml/classopen_p_m_d_1_1_dynamic_memory_view.html
         *
         * The signature of the needed `alpaka::createView()` overload is:
         * `auto createView(TDev const& dev, TElem* pMem, TExtent const& extent, TPitch pitch)`
         */

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
