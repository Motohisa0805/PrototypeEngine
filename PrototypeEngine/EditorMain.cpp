#include "EngineWindow.h"
#include "SDL3.h"
#include <windows.h>
#define DISABLE_DEBUG_NEW // �ꎞ�I�ɖ����ɂ���
#include "Debug_memory.h"
#undef DISABLE_DEBUG_NEW   // �����ɉ���

void CurrentDirectorySetting()
{
	// ���s�t�@�C���̃t���p�X���擾�i��: C:\Users\User\Project\bin\app.exe�j
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string exePath(buf);

	// ���s�t�@�C��������f�B���N�g���𒊏o�i��: C:\Users\User\Project\bin�j
	size_t lastSlashPos = exePath.find_last_of("\\/");
	std::string parentDir = exePath.substr(0, lastSlashPos);

	// �폜������������i\bin�j������
	size_t binPos = parentDir.rfind("\\bin");

	// �����񂪌��������ꍇ
	if (binPos != std::string::npos) {
		// \bin�̕������p�X����폜�i��: C:\Users\User\Project�j
		std::string projectRoot = parentDir.substr(0, binPos);
		SetCurrentDirectory(projectRoot.c_str()); // �J�����g�f�B���N�g�����v���W�F�N�g�̃��[�g�ɕύX
		std::cout << "Project Root: " << projectRoot << std::endl;
	}
	else {
		std::cout << "Could not find '\\bin' in the path." << std::endl;
	}
}


//FOCUS : �v���W�F�N�g�����s���Ă���ꏊ
//�\���}�l�[�W���[��Debug�Ȃ�
#ifdef _DEBUG
int main(int argc, char* argv[])
{
	CurrentDirectorySetting();


	// ���������[�N���o��L���ɂ���
	EnableMemoryLeakCheck();
	// �G���W���̏�����
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	return 0;
}
// �v���W�F�N�g�̎��s�ꏊ��Release�Ȃ�
#elif defined(_RELEASE)
#define SDL_MAIN_HANDLED
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CurrentDirectorySetting();


	// ���������[�N���o��L���ɂ���
	//EnableMemoryLeakCheck();
	// �G���W���̏�����
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	return 0;
}
#endif