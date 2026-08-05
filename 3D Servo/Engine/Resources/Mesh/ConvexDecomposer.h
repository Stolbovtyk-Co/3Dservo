#pragma once
#include <vector>
#include "EngineConstants.h"

namespace VHACD {
    class IVHACD;
    namespace Isorender { class ConvexHull; }
}

class ConvexDecomposer
{
public:
    ConvexDecomposer() = default;
    ~ConvexDecomposer() = default;

    //std::vector<EConst::CpuMeshDTO> runConvexDecomposition(EConst::CpuMeshDTO dto);

private:
    struct vhacdMeshDTO {
        std::vector<double> Points;
        std::vector<uint32_t> Triangles;
    };

    //vhacdMeshDTO ConvertToVHACD(EConst::CpuMeshDTO dto);

    //EConst::CpuMeshDTO ConvertToMesh(void* hullPtr);
};