#pragma once
#include "Component.h"

//書籍部分
//レーダーのUIに表示するためのコンポーネント
class TargetComponent : public Component
{
public:
	TargetComponent(class ActorObject* owner);
	~TargetComponent();
};

