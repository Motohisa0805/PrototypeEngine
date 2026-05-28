#include "Canvas.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "BaseScene.h"

Canvas::Canvas(uint64_t id)
	:UIActorObject(id)
{
	mName = "Canvas";
}

Canvas::~Canvas()
{
}

void Canvas::Update(float deltaTime)
{
	if (mState != EActive)return;

	UIActorObject::Update(deltaTime);

}

/*
void Canvas::DrawTexture(class Shader* shader, class Texture* texture,
	const Vector2& offset, Vector3 scale, float angle)
{
	// Scale the quad by the width/height of texture
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale.x,
		static_cast<float>(texture->GetHeight()) * scale.y,
		scale.z);
	// ‰ñ“]iZŽ²‰ñ“]j
	Matrix4 rotationMat = Matrix4::CreateRotationZ(angle);
	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(
		Vector3(offset.x, offset.y, 0.0f));
	// Set world transform
	Matrix4 world = scaleMat * rotationMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	// Set current texture
	texture->SetActive();
	// Draw quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
*/

