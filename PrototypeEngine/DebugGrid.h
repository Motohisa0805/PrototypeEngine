#pragma once
#include "Typedefs.h"
#include "Math.h"
class VertexArray;
class Shader;

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

