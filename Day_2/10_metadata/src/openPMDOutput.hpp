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
        // TODO: Set global metadata here
        // use methods from
        // https://openpmd-api.readthedocs.io/en/0.16.0/_static/doxyhtml/classopen_p_m_d_1_1_series.html
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

        // TODO: Inspect the file that was produced by this code.
        // It has default values for the metadata.
        // Use adequate API calls to set the metadata.
        // Generally, setting standard-defined attributes works by object.set<attribute_name>(),
        // e.g. for setting `unitSI`: object.setUnitSI(2.0)
        // For the detailed API documentation:
        // Since this is a Mesh Record, but (due to being scalar) also a Record Component,
        // the adequate API calls of this are found at:
        // https://openpmd-api.readthedocs.io/en/0.16.0/_static/doxyhtml/classopen_p_m_d_1_1_mesh.html
        // (Some of them are publically inherited from Record, MeshRecordComponent, RecordComponent,
        //  you might need to open them explicitly on the documentation page.)


        auto logical_extents = alpaka::getExtents(accBuffer);

        openPMD::Dataset dataset_definition{openPMD::determineDatatype<value_t>(), asOpenPMDExtent(logical_extents)};
        image.resetDataset(dataset_definition);

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
