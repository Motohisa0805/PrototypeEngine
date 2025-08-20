#pragma once
#include "StandardLibrary.h"
#include "Shader.h"
#include "VertexArray.h"

//�O���b�h��`�悷��N���X
class DebugGrid
{
private:

	VertexArray* mGridVAO;
public:
	DebugGrid(float gridSize = 1.0f, int gridCount = 50);
	~DebugGrid();

	void Draw(Shader* shader, const Matrix4& viewProj);
};

