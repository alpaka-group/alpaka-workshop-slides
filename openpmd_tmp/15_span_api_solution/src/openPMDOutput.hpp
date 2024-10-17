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
        openPMD::Mesh image = current_iteration.meshes["heat"];

        image.setAxisLabels({"x", "y"});
        image.setGridGlobalOffset({0., 0.});
        image.setGridSpacing(std::vector<double>{1., 1.});
        image.setGridUnitSI(1.0);
        image.setPosition(std::vector<double>{0.5, 0.5});
        image.setUnitDimension({{openPMD::UnitDimension::theta, 1.0}});
        image.setUnitSI(1.0);

        auto logical_extents = alpaka::getExtents(accBuffer);

        image.resetDataset({openPMD::determineDatatype<value_t>(), asOpenPMDExtent(logical_extents)});


        auto openPMDBuffer = image.storeChunk<value_t>({0, 0}, asOpenPMDExtent(logical_extents));
        auto bufferView = alpaka::createView(devHost, openPMDBuffer.currentBuffer().data(), logical_extents);
        alpaka::memcpy(dumpQueue, bufferView, accBuffer);
        alpaka::wait(dumpQueue);

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
