#pragma once
#include "Component.h"


//エンジンでは派生クラスとして拡張性がなくなるの廃止予定

//書籍部分
//レーダーのUIに表示するためのコンポーネント
class TargetComponent : public Component
{
public:
	TargetComponent(ActorObject* owner);
	~TargetComponent();
};

