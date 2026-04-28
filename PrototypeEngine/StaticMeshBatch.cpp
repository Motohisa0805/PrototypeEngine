#include "StaticMeshBatch.h"
/*
StaticMeshBatch::StaticMeshBatch()
{
}

StaticMeshBatch::~StaticMeshBatch()
{
}

void StaticMeshBatch::BuildStaticBatch(vector<MeshRenderer*> meshComps)
{
	vector<Vertex> combinedVertices;
	vector<unsigned int> combinedIndices;
	unsigned int vertexOffset = 0;

	for (auto mc : meshComps) {
		if (mc->GetOwner()->GetStatic() != ActorInformation::StaticTag::Occluder_Static) {
			continue;
		}

		Matrix4 worldTrans = mc->GetOwner()->GetTransform()->GetWorldTransform();
		for (auto mesh : mc->GetMeshs()) {
			// 元の頂点データを取得（MeshクラスにGetterが必要）
			const vector<Vertex>& srcVertices = mesh->GetVertices();
			const vector<unsigned int>& srcIndices = mesh->GetIndices();

			// 頂点を変換して結合
			for (const auto& v : srcVertices) {
				Vertex newVert = v;
				// ワールド変換を適用
				newVert.pos = Vector3::Transform(v.pos, worldTrans);
				newVert.normal = Vector3::TransformNormal(v.normal, worldTrans);

				combinedVertices.push_back(newVert);
			}
			// インデックスを結合（オフセットを考慮）
			for (unsigned int idx : srcIndices) {
				combinedIndices.push_back(idx + vertexOffset);
			}
			vertexOffset += static_cast<unsigned int>(srcVertices.size());
		}
	}
	// 結合された頂点とインデックスで新しいMeshを作成
	if (!combinedVertices.empty()) {
		mBatchVertexArray = new VertexArray(combinedVertices.data(), combinedVertices.size(), VertexArray::PosNormTex, combinedIndices.data(), combinedIndices.size());
	}
}

void StaticMeshBatch::Draw(Shader* shader, const Matrix4& viewProj, const Vector3& cameraPos)
{
	if(!mBatchVertexArray) {
		return;
	}
	shader->SetMatrixUniform("uWorldTransform", Matrix4::Identity);
	mBatchVertexArray->SetActive();
	glDrawElements(GL_TRIANGLES, mBatchVertexArray->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

*/