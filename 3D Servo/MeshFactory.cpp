#include "MeshFactory.h"
#include "MeshInstance3D.h"

std::shared_ptr<Node3D> MeshFactory::CreateStaticInstance(std::string PATH)
{
    auto newInstance = std::make_shared<MeshInstance3D>();

    newInstance->SetSubMeshes(m_objMgr->GetObjectSubMeshes(PATH));

    return newInstance;
}