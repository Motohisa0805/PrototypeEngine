#include "ComponentFactory.h"
#include "Rigidbody.h"
#include "MeshRenderer.h"
#include "SkeletalMeshRenderer.h"
#include "ParticleSystem.h"
#include "DirectionalLightComponent.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "AudioComponent.h"
#include "Animator.h"

#include "BasicInputAction.h"
#include "FollowObjectMovement.h"
#include "PushButtonMove.h"
#include "TargetComponent.h"

#include "BaseCamera.h"
#include "FreeCamera.h"
#include "FollowCamera.h"
#include "FPSCamera.h"

std::map<string, ComponentCreator> ComponentFactory::sCreators;

void ComponentFactory::RegisterComponent(const string& type, ComponentCreator creator)
{
	sCreators[type] = creator;
}

Component* ComponentFactory::CreateComponent(const string& type, ActorObject* owner)
{
	auto it = sCreators.find(type);
	if (it != sCreators.end())
	{
		return it->second(owner);//生成関数を実行
	}
	return nullptr;
}

std::vector<string> ComponentFactory::GetRegisteredComponentNames()
{
	vector<string> names;
	for (const auto& pair : sCreators)
	{
		names.push_back(pair.first);
	}
	return names;
}

// --------------------------------------------------
//  コンポーネントの登録
// --------------------------------------------------
// 各コンポーネントファイルで定義しても良いが、ここでは一箇所にまとめる
// REGISTER_COMPONENT(MeshRenderer, "MeshRenderer"); の代わりに直接呼び出す例:
// 現状GUIで使用できるコンポーネントだけ処理
void RegisterAllComponents()
{
	//ComponentFactory::RegisterComponent("Rigidbody",[](ActorObject* owner) -> Component* { return new Rigidbody(owner); });
	
	ComponentFactory::RegisterComponent("MeshRenderer",[](ActorObject* owner) -> Component* { return new MeshRenderer(owner, false); });
	
	ComponentFactory::RegisterComponent("SkeletalMeshRenderer",[](ActorObject* owner) -> Component* { return new SkeletalMeshRenderer(owner); });
	
	//ComponentFactory::RegisterComponent("ParticleSystem",[](ActorObject* owner) -> Component* { return new ParticleSystem(owner); });
	
	ComponentFactory::RegisterComponent("DirectionalLightComponent",[](ActorObject* owner) -> Component* { return new DirectionalLightComponent(owner); });
	
	//ComponentFactory::RegisterComponent("BoxCollider",[](ActorObject* owner) -> Component* { return new BoxCollider(owner); });
	
	//ComponentFactory::RegisterComponent("CapsuleCollider",[](ActorObject* owner) -> Component* { return new CapsuleCollider(owner); });
	
	//ComponentFactory::RegisterComponent("SphereCollider",[](ActorObject* owner) -> Component* { return new SphereCollider(owner); });
	
	//ComponentFactory::RegisterComponent("AudioComponent",[](ActorObject* owner) -> Component* { return new AudioComponent(owner); });
	
	//ComponentFactory::RegisterComponent("Animator",[](ActorObject* owner) -> Component* { return new Animator(owner); });

	//ComponentFactory::RegisterComponent("BasicInputAction",[](ActorObject* owner) -> Component* { return new BasicInputAction(owner); });
	
	//ComponentFactory::RegisterComponent("FollowObjectMovement",[](ActorObject* owner) -> Component* { return new FollowObjectMovement(owner); });
	
	//ComponentFactory::RegisterComponent("PushButtonMove",[](ActorObject* owner) -> Component* { return new PushButtonMove(owner); });
	
	//ComponentFactory::RegisterComponent("TargetComponent",[](ActorObject* owner) -> Component* { return new TargetComponent(owner); });
	
	//ComponentFactory::RegisterComponent("BaseCamera",[](ActorObject* owner) -> Component* { return new BaseCamera(owner); });
	
	ComponentFactory::RegisterComponent("FreeCamera",[](ActorObject* owner) -> Component* { return new FreeCamera(owner); });
	
	//ComponentFactory::RegisterComponent("FollowCamera",[](ActorObject* owner) -> Component* { return new FollowCamera(owner); });
	
	//ComponentFactory::RegisterComponent("FPSCamera",[](ActorObject* owner) -> Component* { return new FPSCamera(owner); });
	// ...他のコンポーネントも同様に追加
}