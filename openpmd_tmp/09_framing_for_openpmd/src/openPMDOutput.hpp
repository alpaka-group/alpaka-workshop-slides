#pragma once

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
        m_series = openPMD::Series("heat.bp5", openPMD::Access::CREATE);
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

        // TODO: Navigate to correct object in the openPMD hierarchy.
        // Reference: https://openpmd-api.readthedocs.io/en/0.16.0/usage/concepts.html

        auto logical_extents = alpaka::getExtents(accBuffer);

        // TODO: Use resetDataset() for specifying the 2D array's datatype and extent.

        alpaka::memcpy(dumpQueue, hostBuffer, accBuffer);
        alpaka::wait(dumpQueue);

        // TODO: Write 2D data to openPMD.

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
