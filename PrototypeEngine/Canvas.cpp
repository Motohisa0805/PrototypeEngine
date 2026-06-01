#include "Canvas.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "BaseScene.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "WindowRenderProperty.h"

Canvas::Canvas(uint64_t id)
	:UIActorObject(id)
{
	mIsCanvas = true;
	mName = "Canvas";

	mRectTransform->SetScaleWidthAndHeight(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());

	EngineWindow::GetRenderer()->AddCanvasActor(this);
}

Canvas::Canvas(BaseScene* scene)
	:UIActorObject(scene)
{
	mIsCanvas = true;
	mName = "Canvas";

	mRectTransform->SetScaleWidthAndHeight(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());

	EngineWindow::GetRenderer()->AddCanvasActor(this);
}

Canvas::~Canvas()
{
	EngineWindow::GetRenderer()->RemoveCanvasActor(this);
}

void Canvas::Update(float deltaTime)
{
	if (mState != EActive)return;

	UIActorObject::Update(deltaTime);

}
