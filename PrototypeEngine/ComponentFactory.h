#pragma once
#include "Component.h"
#include <string>
#include <map>
#include <functional>

class Component;
class ActorObject;

using ComponentCreator = std::function<Component* (ActorObject*)>;

//FOCUS : �R���|�[�l���g�̓��I�������s���Ă���t�@�C��
class ComponentFactory
{
private:
	//�R���|�[�l���g���ƁA����𐶐�����֐��̃}�b�v
	static std::map<string, ComponentCreator> sCreators;
public:
	//�t�@�N�g���[�̓o�^����
	static void					RegisterComponent(const string& type, ComponentCreator creator);
	
	static Component*			CreateComponent(const string& type, ActorObject* owner);

	//�o�^����Ă���R���|�[�l���g�̈ꗗ���擾
	static std::vector<string>	GetRegisteredComponentNames();
};

// --------------------------------------------------
// �o�^���ȗ������邽�߂̃w���p�[�N���X/�}�N�� (Optional)
// --------------------------------------------------
template<typename T>
struct  ComponentRegistrar
{
	ComponentRegistrar(const string& name)
	{
		ComponentFactory::RegisterComponent(name,
			[](ActorObject* owner) -> Component*
			{
				return new T(owner);
			});
	}
};

//�g�p��:REGISTER_COMPONET(MeshRenderer,"MeshRenderer");
#define REGISTER_COMPONET(Class,Name) static ComponentRegistrar<Class> reg_##Class(Name);

extern void RegisterAllComponents();
