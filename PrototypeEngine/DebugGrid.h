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
	//現在のグリッド間隔
	float mCurrentGridSize;
	//現在のグリッド数
	int mCurrentGridCount;
	//現在のグリッドの中心(X,0,Z)
	Vector3 mGridCenter;

	void GenerateGrid(float gridSize, int gridCount,const Vector3& center);
public:
	DebugGrid(float gridSize = 1.0f, int gridCount = 50);
	~DebugGrid();

	void Draw(Shader* shader, const Matrix4& viewProj, const Vector3& cameraPos);
};

