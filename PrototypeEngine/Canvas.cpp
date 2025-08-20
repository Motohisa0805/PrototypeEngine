#include "Canvas.h"


Canvas::Canvas()
	:mGame(GameApp::GetActiveScene())
	, mTitle(nullptr)
	, mBackground(nullptr)
	, mTitlePos(0.0f, 300.0f)
	, mNextButtonPos(0.0f, 200.0f)
	, mBGPos(0.0f, 250.0f)
	, mState(EActive)
{
	// Add to UI Stack
	mGame->PushUI(this);

	mButtonOn = new Texture();
	mButtonOn->Load(TexFile::TextureFilePath + "ButtonYellow.png");

	mButtonOff = new Texture();
	mButtonOff->Load(TexFile::TextureFilePath + "ButtonBlue.png");
}

Canvas::~Canvas()
{
	for (auto b : mButtons)
	{
		delete b;
	}
	mButtons.clear();

	if(mTitleImage)
	{
		mGame->RemoveImage(mTitleImage);
		delete mTitleImage;
	}
	if (mTitleFont)
	{
		mGame->RemoveImage(mTitleFont);
		delete mTitleFont;
	}
	if (mBackground)
	{
		mGame->RemoveImage(mBackground);
		delete mBackground;
	}

	if (mButtonOff)
	{
		delete mButtonOff;
	}
	if (mButtonOn)
	{
		delete mButtonOn;
	}
}

void Canvas::Update(float deltaTime)
{
	// Draw background (if exists)
	if (mBackground)
	{
		mBackground->SetPosition(mBGPos);
	}
	// Draw title (if exists)
	if (mTitleFont)
	{
		mTitleFont->SetPosition(mTitlePos);
	}
	// Draw buttons
	for (auto b : mButtons)
	{
		// Draw background of button
		//ƒ{ƒ^ƒ“‚Ì˜g‚ð•`‰æ
		if (b->GetHighlighted())
		{
			b->SetButtonText(mButtonOn);
		}
		else
		{
			b->SetButtonText(mButtonOff);
		}
	}
	// Override in subclasses to draw any textures
}

void Canvas::Draw(Shader* shader)
{
	
}

void Canvas::ProcessInput(const struct InputState& keys)
{
	// Do we have buttons?
	if (!mButtons.empty())
	{
		// Get position of mouse
		float x, y;
		SDL_GetMouseState(&x, &y);
		// Convert to (0,0) center coordinates
		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
		mousePos.x -= WindowRenderProperty::GetWidth() * 0.5f;
		mousePos.y = WindowRenderProperty::GetHeight() * 0.5f - mousePos.y;

		// Highlight any buttons
		for (auto b : mButtons)
		{
			if (b->ContainsPoint(mousePos))
			{
				b->SetHighlighted(true);
			}
			else
			{
				b->SetHighlighted(false);
			}

			if( b->GetHighlighted() && keys.Mouse.GetButtonDown(SDL_BUTTON_LEFT))
			{
				b->OnClick();
			}
		}
	}
}

void Canvas::Close()
{
	mState = EDestroy;
}

void Canvas::SetState(UIState state)
{
	mState = state;

}

void Canvas::SetTitle(const string& text,
	const Vector3& color,
	int pointSize)
{
	// Clear out previous title texture if it exists
	mTitleFont->SetColor(color);
	mTitleFont->SetFontSize(pointSize);
	mTitleFont->SetText(text);
}

void Canvas::AddButton(const string& name, std::function<void()> onClick)
{
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()),
		static_cast<float>(mButtonOn->GetHeight()));
	Button* b = new Button(name, mTitleFont->GetFont(), onClick, mNextButtonPos, dims);
	mButtons.emplace_back(b);

	// Update position of next button
	// Move down by height of button plus padding
	mNextButtonPos.y -= mButtonOff->GetHeight() + 20.0f;
}

Button* Canvas::CreateButton(const string& name, const Vector2& pos, std::function<void()> onClick)
{
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()),
		static_cast<float>(mButtonOn->GetHeight()));
	Button* b = new Button(name, mTitleFont->GetFont(), onClick, pos, dims);
	//AddChildUIImage((Image*)b);
	mButtons.emplace_back(b);
	return b;
}

Button* Canvas::CreateButton(const char8_t* name, const Vector2& pos, std::function<void()> onClick)
{
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()),
		static_cast<float>(mButtonOn->GetHeight()));
	Button* b = new Button(name, mTitleFont->GetFont(), onClick, pos, dims);
	//AddChildUIImage((Image*)b);
	mButtons.emplace_back(b);
	return b;
}



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

void Canvas::SetRelativeMouseMode(bool relative)
{
	if (SceneManager::IsLoading()) { return; }
	if (relative)
	{
		SDL_SetWindowRelativeMouseMode(GameWinMain::GetRenderer()->GetWindow(), true);
		// Make an initial call to get relative to clear out
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		SDL_SetWindowRelativeMouseMode(GameWinMain::GetRenderer()->GetWindow(),false);
	}
}

void Canvas::AddChildUIImage(Image* image)
{
	mImages.emplace_back(image);
}

