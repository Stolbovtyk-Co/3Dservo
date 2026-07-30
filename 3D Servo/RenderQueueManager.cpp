#include "RenderQueueManager.h"
#include <algorithm> 

EConst::GPUIstructionsDTO RenderQueueManager::MakeRenderQueue()
{
	EConst::GPUIstructionsDTO output;

	std::vector<EConst::Instruction> nodeInstructions = m_scenePtr->GetGPUInstructions();

	std::vector<EConst::GPUBuffers> regular;
	std::vector<EConst::GPUBuffers> transparent;

	DirectX::BoundingFrustum frustum;

	DirectX::BoundingFrustum::CreateFromMatrix(frustum, DirectX::XMLoadFloat4x4(&(m_bufferStruct->projection)), true);
	auto invViewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&(m_bufferStruct->view)));

	frustum.Transform(frustum, invViewMatrix);
	DirectX::XMVECTOR cameraPos = invViewMatrix.r[3];

	for (auto& inst : nodeInstructions) {

		auto cWorldMatrix = DirectX::XMLoadFloat4x4(&(inst.world));
		auto cTransparent = inst.SV_TRANSPARENT;

		for (auto& mesh : inst.subMeshes) {

			DirectX::BoundingBox worldBox;
			mesh.box.Transform(worldBox, cWorldMatrix);

			if (frustum.Contains(worldBox) == DirectX::DISJOINT) {
				// WARNING! VIEW CLIPPING COUNTED WRONG! TODO: fix
				//	continue; 
			}

			DirectX::XMVECTOR boxCenter = DirectX::XMLoadFloat3(&worldBox.Center);
			float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(boxCenter, cameraPos)));

			EConst::GPUBuffers gBuff;
			gBuff.indexCount = mesh.indexCount;
			gBuff.cDistSqr = distSq;
			gBuff.iBuffer = mesh.iBuffer;
			gBuff.vBuffer = mesh.vBuffer;
			gBuff.worldMatrix = cWorldMatrix;

			if (cTransparent)
			{
				transparent.push_back(gBuff);
			}
			else
			{
				regular.push_back(gBuff);
			}
		}
	}

	std::sort(regular.begin(), regular.end(), [](const EConst::GPUBuffers& a, const EConst::GPUBuffers& b) {
		return a.cDistSqr < b.cDistSqr;
		});
	std::sort(transparent.begin(), transparent.end(), [](const EConst::GPUBuffers& a, const EConst::GPUBuffers& b) {
		return a.cDistSqr > b.cDistSqr;
		});

	output.regular = regular;
	output.transparent = transparent;

	return output;
}
