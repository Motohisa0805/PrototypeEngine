#pragma once
#include "PrototypeEngine_API.h"
#include "Typedefs.h"

class Component;
class Entity;

using ComponentCreator = std::function<Component* (Entity*)>;

//FOCUS : コンポーネントの動的生成を行っているファイル
class PROTOTYPEENGINE_API ComponentFactory
{
private:
	//コンポーネント名と、それを生成する関数のマップ
	static std::map<string, ComponentCreator> sCreators;
public:
	//ファクトリーの登録処理
	static void					RegisterComponent(const string& type, ComponentCreator creator);
	
	static Component*			CreateComponent(const string& type, Entity* owner);

	//登録されているコンポーネントの一覧を取得
	static std::vector<string>	GetRegisteredComponentNames();
	static void					UnregisterAllComponents();
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
			[](Entity* owner) -> Component*
			{
				return new T(owner);
			});
	}
};

//使用例:REGISTER_COMPONET(MeshRenderer,"MeshRenderer");
#define REGISTER_COMPONET(Class,Name) static ComponentRegistrar<Class> reg_##Class(Name);

extern void RegisterAllComponents();

//新しいユーザー定義スクリプト用マクロを定義。
//クラス名(ClassName)を文字列化(#ClassName)して自動登録
#define REGISTER_SCRIPT_COMPONENT(ClassName) \
	namespace { \
		static ComponentRegistrar<ClassName> reg_script_##ClassName(#ClassName); \
    }
