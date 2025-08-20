#include "Shader.h"


Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{

}

Shader::~Shader()
{

}

bool Shader::Load(const string& vertName, const string& fragName)
{
	string vertPath = File_P::ShaderPath + vertName;
	string fragPath = File_P::ShaderPath + fragName;
	// 頂点シェーダーとピクセルシェーダーをコンパイルする
	if (!CompileShader(vertPath,
		GL_VERTEX_SHADER,
		mVertexShader) ||
		!CompileShader(fragPath,
			GL_FRAGMENT_SHADER,
			mFragShader))
	{
		return false;
	}

	// 頂点
	// フラグシェーダーをリンクするシェーダープログラムを作成。
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// プログラムが正常にリンクしたことを確認してください
	if (!IsValidProgram())
	{
		return false;
	}

	return true;
}

void Shader::Unload()
{
	// プログラム/シェーダーを削除する
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
	// このプログラムをアクティブなものとして設定します
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// この名前のUniformLocationを見つける
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列データをユニフォームに送信
	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
}

void Shader::SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列データをユニフォームに送信
	glUniformMatrix4fv(loc, count, GL_TRUE, matrices->GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// ベクトルデータを送信
	glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVector4Uniform(const char* name, const Vector4& vector)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// ベクトルデータを送信
	glUniform4fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVector2Uniform(const char* name, const Vector2& vector)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// ベクトルデータを送信
	glUniform2fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char* name, float value)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 浮動小数点データを送信
	glUniform1f(loc, value);
}

void Shader::SetIntUniform(const char* name, int value)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 浮動小数点データを送信
	glUniform1i(loc, value);
}

void Shader::SetMaterialColor(const Vector4& color)
{
	mMaterialColor = color;
}

void Shader::SetColorUniform(const char* name, const MaterialInfo& info)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, "ambientColor");
	glUniform3f(loc, info.Ambient.x, info.Ambient.y, info.Ambient.z);

	loc = glGetUniformLocation(mShaderProgram, "diffuseColor");
	glUniform3f(loc, info.Diffuse.x, info.Diffuse.y, info.Diffuse.z);

	loc = glGetUniformLocation(mShaderProgram, "specularColor");
	glUniform3f(loc, info.Specular.x, info.Specular.y, info.Specular.z);

	loc = glGetUniformLocation(mShaderProgram, "shininess");
	glUniform1f(loc, info.Shininess);

	// uColor（RGBA）を送る
	loc = glGetUniformLocation(mShaderProgram, "uColor");
	glUniform4f(loc, info.Color.x, info.Color.y, info.Color.z, info.Color.w);
	/*
	loc = glGetUniformLocation(mShaderProgram, name);
	glUniform4fv(loc, 1, mMaterialColor.GetAsFloatPtr());
	*/
}

void Shader::SetMaterialUniform(const char* name, const Vector3& color)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	glUniform3f(loc, color.x, color.y, color.z);
}

void Shader::SetNoTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Shader::CompileShader(const string& fileName,
	GLenum shaderType,
	GLuint& outShader)
{
	// Open file
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// すべてのテキストを文字列に読み込み
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// 指定されたタイプのシェーダーを作成します。
		outShader = glCreateShader(shaderType);
		// ソース文字を設定して、コンパイルを試みる
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			SDL_Log("Failed to compile shader %s", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("Shader file not found: %s", fileName.c_str());
		return false;
	}

	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// コンパイル状況を照会する
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSL Compile Failed:\n%s", buffer);
		return false;
	}

	return true;
}

bool Shader::IsValidProgram()
{

	GLint status;
	// リンクの状態を照会する
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}

	return true;
}

