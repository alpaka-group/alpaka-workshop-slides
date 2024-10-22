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
        m_series = openPMD::Series("openpmd/heat_%06T.bp5", openPMD::Access::CREATE);
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
        // Reference:
        // Concepts: https://openpmd-api.readthedocs.io/en/0.16.0/usage/concepts.html
        // API:      https://openpmd-api.readthedocs.io/en/0.16.0/usage/firstwrite.html

        auto logical_extents = alpaka::getExtents(accBuffer);

        // TODO: We need to define the shape and extent of the dataset in `dataset_definition` before writing it.
        // It needs the correct datatype, and the shape/extent of the image.
        // For determining the datatype, use `openPMD::determineDatatype<>()`.
        // The extent is given as `openPMD::Extent` which is just a `std::vector`.
        // Use the `asOpenPMDExtent()` helper function to convert from the custom Alpaka function.
        //
        // openPMD::Dataset dataset_definition{ ... fill this in ... };
        //
        // TODO: Use resetDataset() for specifying the 2D array's datatype and extent.

        alpaka::memcpy(dumpQueue, hostBuffer, accBuffer);
        alpaka::wait(dumpQueue);

        // TODO: Write 2D data to openPMD. Use `RecordComponent::storeChunkRaw()` for writing from raw pointers.
        // Then inspect the dataset:
        //
        // 1. with `bpls openpmd/heat_000000.bp5` (native ADIOS2 tooling)
        // 2. and with `openpmd-ls openpmd/heat_%06T.bp5` (openPMD-api tooling).

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
