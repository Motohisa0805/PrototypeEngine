#pragma once
#include "StandardLibrary.h"
#include "Shader.h"
#include "VertexArray.h"

//グリッドを描画するクラス
class DebugGrid
{
private:

	VertexArray* mGridVAO;
public:
	DebugGrid(float gridSize = 1.0f, int gridCount = 50);
	~DebugGrid();

	void Draw(Shader* shader, const Matrix4& viewProj);
};

