#pragma once
#include "Math.h"
#include "Mesh.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元を改造したファイル
//シェーダーの読み込みクラス
class Shader
{
private:
	// 指定されたシェーダーのコンパイルを試みます
	bool		CompileShader(const string& fileName,
		GLenum shaderType,
		GLuint& outShader);

	// シェーダーが正常にコンパイルされたかどうかをテスト
	bool		IsCompiled(GLuint shader);
	// 頂点/フラグメントプログラムがリンクされるかどうかをテスト
	bool		IsValidProgram();

	// シェーダーオブジェクトIDを保存する
	GLuint		mVertexShader;
	GLuint		mFragShader;
	GLuint		mShaderProgram;
	Vector4		mMaterialColor;
public:
				Shader();
				~Shader();

	bool		Load(const string& vertName, const string& fragName);
	void		Unload();
	// これをアクティブシェーダープログラムとして設定します。
	void		SetActive();
	// マトリックスの uniform を設定
	void		SetMatrixUniform(const char* name, const Matrix4& matrix);
	// マトリックス uniform の配列を設定
	void		SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count);
	// Vector3のユニフォームを設定
	void		SetVectorUniform(const char* name, const Vector3& vector);
	void		SetVector4Uniform(const char* name, const Vector4& vector);
	void		SetVector2Uniform(const char* name, const Vector2& vector);
	// 浮動小数点数のユニフォームを設定
	void		SetFloatUniform(const char* name, float value);
	// 整数のユニフォームを設定
	void		SetIntUniform(const char* name, int value);

	void		SetMaterialColor(const Vector4& color);

	void		SetColorUniform(const char* name,const MaterialInfo& info);

	void		SetMaterialUniform(const char* name, const Vector3& color);

	void		SetNoTexture();
};
