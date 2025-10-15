#pragma once
#include "StandardLibrary.h"
#include "Typedefs.h"

class BaseScene;
class ActorObject;

class SceneSerializer
{
private:
public:
	// ��������̃A�N�^�[���X�g���󂯎��A�w��p�X�ɃV�[����ۑ�����
	static bool SaveScene(const filesystem::path& filePath, BaseScene* scene);
	static bool SaveEmptyScene(const filesystem::path& filePath);
	static BaseScene* LoadScene(const string& filePath);
};

