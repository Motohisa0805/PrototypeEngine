#pragma once
#include "Component.h"
#include <string>
#include <map>
#include <functional>

class Component;
class ActorObject;

using ComponentCreator = std::function<Component* (ActorObject*)>;

//FOCUS : コンポーネントの動的生成を行っているファイル
class ComponentFactory
{
private:
	//コンポーネント名と、それを生成する関数のマップ
	static std::map<string, ComponentCreator> sCreators;
public:
	//ファクトリーの登録処理
	static void					RegisterComponent(const string& type, ComponentCreator creator);
	
	static Component*			CreateComponent(const string& type, ActorObject* owner);

	//登録されているコンポーネントの一覧を取得
	static std::vector<string>	GetRegisteredComponentNames();
};

// --------------------------------------------------
// 登録を簡略化するためのヘルパークラス/マクロ (Optional)
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

//使用例:REGISTER_COMPONET(MeshRenderer,"MeshRenderer");
#define REGISTER_COMPONET(Class,Name) static ComponentRegistrar<Class> reg_##Class(Name);

extern void RegisterAllComponents();
