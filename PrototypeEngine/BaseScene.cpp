#include "BaseScene.h"
#include "GameFunctions.h"

BaseScene::BaseScene()
	: mAudioSystem(nullptr)
	, mPhysWorld(nullptr)
	, mUpdatingActors(false)
	, mFixed_Delta_Time(0.02f)
	, mPlayer(nullptr)
	, mMainCamera(nullptr)
	, mFixedTimeAccumulator(0.0f)
	, mDirectionalLightActor(nullptr)
	, mFrameRateText(nullptr)
{
}

void BaseScene::Shutdown()
{
	//ゲーム終了時の解放処理
	InputSystem::Shutdown();
}

void BaseScene::SetMouseMode(MouseMode mode)
{
	if (mode == MouseMode::Relative)
	{
		SDL_SetWindowRelativeMouseMode(GameWinMain::GetRenderer()->GetWindow(), true);
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else if (mode == MouseMode::Absolute)
	{
		SDL_SetWindowRelativeMouseMode(GameWinMain::GetRenderer()->GetWindow(), false);
	}
	else
	{
		SDL_Log("Unknown mouse mode");
	}
}

void BaseScene::LoadSkyBoxTexture(string file)
{
	GameWinMain::GetRenderer()->GetSkyBoxRenderer()->Load(file);
}

bool BaseScene::Initialize()
{
	if (!InputSystem::Initialize())
	{
		SDL_Log("Failed to initialize input system");
		return false;
	}

	// Audio Systemを作成
	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize audio system");
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}

	mDirectionalLightActor = new DirectionalLightActor();


	// Physics Worldを作成
	mPhysWorld = new PhysWorld(this);

	Font* font = GetFont("NotoSansJP-Bold.ttf");

	mFrameRateText = new Text(font, Vector2(500, 250),Debug_Function);
	mFrameRateText->SetFontSize(22);
	return true;
}

bool BaseScene::InputUpdate(const InputState& state)
{

	if (GameStateClass::mGameState == GameState::GamePlay)
	{
		for (auto actor : mActors)
		{
			if (actor->GetState() == ActorObject::EActive)
			{
				actor->ProcessInput(state);
			}
		}
	}

#ifdef _DEBUG
	// Debugビルドの場合の処理
	if (state.Keyboard.GetKeyDown(SDL_SCANCODE_F1))
	{
		GameStateClass::mDebugFrag = !GameStateClass::mDebugFrag;
	}
#else
	// Releaseビルドの場合の処理
#endif


	return true;
}

bool BaseScene::FixedUpdate()
{
	float deltaTime = Time::gDeltaTime;
	mFixedTimeAccumulator += deltaTime;

	// 複数回 FixedUpdate が必要な場合に備える
	while (mFixedTimeAccumulator >= mFixed_Delta_Time)
	{
		//Rigidbody などの物理処理をここで呼ぶ
		
		for (auto actor : mActors)
		{
			actor->FixedUpdate(Time::gDeltaTime);
		}

		mPhysWorld->SweepAndPruneXYZ();

		mFixedTimeAccumulator -= mFixed_Delta_Time;
	}

	return true;
}

bool BaseScene::Update()
{
	//特定のシーンに読み込まれたオブジェクトやコンポーネントを
	// まとめて処理する部分
	// Update all actors
	mUpdatingActors = true;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorObject::EActive)
		{
			mActors[i]->Update(Time::gDeltaTime);
		}
	}

	mUpdatingActors = false;

	// 保留中のアクターをmActorsに移動します
	for (int i = 0; i < mPendingActors.size(); i++)
	{
		mPendingActors[i]->ComputeWorldTransform(NULL);
		mActors.emplace_back(mPendingActors[i]);
	}

	mPendingActors.clear();

	// Add any dead actors to a temp vector
	vector<ActorObject*> deadActors;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorObject::EDead)
		{
			deadActors.emplace_back(mActors[i]);
		}
	}


	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}

	// Update audio system
	mAudioSystem->Update(Time::gDeltaTime);

	// Update UI screens
	for (int i = 0; i < mCanvasStack.size(); i++)
	{
		if (mCanvasStack[i]->GetState() == Canvas::EActive)
		{
			mCanvasStack[i]->Update(Time::gDeltaTime);
		}
	}

	for (int i = 0; i < mImageStack.size(); i++)
	{
		if (mImageStack[i]->GetState() == Image::EActive)
		{
			mImageStack[i]->Update(Time::gDeltaTime);
		}
	}

	if (GameStateClass::mDebugFrag)
	{
		for (int i = 0; i < mDebugImageStack.size(); i++)
		{
			if (mDebugImageStack[i]->GetState() == Image::EActive)
			{
				mDebugImageStack[i]->Update(Time::gDeltaTime);
			}
		}
		float time = Time::GetFrameRate();
		mFrameRateText->SetText("FPS : " + FloatToString::ToStringWithoutDecimal(time));
	}

	// Delete any UIScreens that are closed
	auto iter = mCanvasStack.begin();
	while (iter != mCanvasStack.end())
	{
		if ((*iter)->GetState() == Canvas::EDestroy)
		{
			delete* iter;
			iter = mCanvasStack.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	auto image = mImageStack.begin();
	while (image != mImageStack.end())
	{
		if ((*image)->GetState() == Image::EDestroy)
		{
			delete* image;
			image = mImageStack.erase(image);
		}
		else
		{
			++image;
		}
	}

	return true;
}

void BaseScene::AddActor(ActorObject* actor)
{
	// If we're updating actors, need to add to pending
	mPendingActors.emplace_back(actor);
	/*
	if (mUpdatingActors)
	{
	}
	else
	{
		mActors.emplace_back(actor);
	}
	*/
}

void BaseScene::RemoveActor(ActorObject* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

Font* BaseScene::GetFont(const string& fileName)
{
	string filePath = FontFile::FontFilePath + fileName;
	auto iter = mFonts.find(filePath);
	if (iter != mFonts.end())
	{
		return iter->second;
	}

	// 新規読み込み
	Font* font = new Font();
	if (!font->Load(filePath))
	{
		// Load失敗時はクリーンに削除
		delete font;
		return nullptr;
	}

	// 成功時のみ map に登録
	mFonts.emplace(filePath, font);
	return font;
}

Skeleton* BaseScene::GetSkeleton(const string& fileName)
{
	string file = File_P::ModelPath + fileName;
	auto iter = mSkeletons.find(file);
	//すでにあるならそれを使う
	if (iter != mSkeletons.end())
	{
		return iter->second;
	}
	//新しく読み込み
	else
	{
		Skeleton* sk = new Skeleton();
		if (sk->LoadFromSkeletonBin(file))
		{
			mSkeletons.emplace(file, sk);
		}
		else if (sk->Load(file))
		{
			mSkeletons.emplace(file, sk);
		}
		else
		{
			delete sk;
			sk = nullptr;
		}
		return sk;
	}
	//読み込み失敗
	return nullptr;
}
//キャンバスを格納
void BaseScene::PushUI(Canvas* screen)
{
	mCanvasStack.emplace_back(screen);
}
//2D画像を格納
void BaseScene::PushImage(Image* screen)
{
	mImageStack.emplace_back(screen);
}
void BaseScene::RemoveImage(Image* screen)
{
	// Is it in actors?
	auto iter = std::find(mImageStack.begin(), mImageStack.end(), screen);
	if (iter != mImageStack.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mImageStack.end() - 1);
		mImageStack.pop_back();
	}
}

void BaseScene::PushDebugImage(Image* screen)
{
	mDebugImageStack.emplace_back(screen);
}

void BaseScene::RemoveDebugImage(Image* screen)
{
	// Is it in actors?
	auto iter = std::find(mDebugImageStack.begin(), mDebugImageStack.end(), screen);
	if (iter != mDebugImageStack.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mDebugImageStack.end() - 1);
		mDebugImageStack.pop_back();
	}
}

void BaseScene::UnloadData()
{
	//シーン内のデータを全解放
	
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	mActors.clear();

	// Clear the UI stack
	while (!mCanvasStack.empty())
	{
		delete mCanvasStack.back();
		mCanvasStack.pop_back();
	}
	mCanvasStack.clear();

	while (!mImageStack.empty())
	{
		delete mImageStack.back();
		mImageStack.pop_back();
	}
	mImageStack.clear();

	while (!mDebugImageStack.empty())
	{
		delete mDebugImageStack.back();
		mDebugImageStack.pop_back();
	}
	mDebugImageStack.clear();

	// Unload fonts
	for (auto& f : mFonts)
	{
		if (f.second)
		{
			f.second->Unload();
			delete f.second;
			f.second = nullptr;
		}
	}
	mFonts.clear();

	// Unload skeletons
	for (auto s : mSkeletons)
	{
		if (s.second) 
		{
			delete s.second;
			s.second = nullptr;
		}
	}
	mSkeletons.clear();
	
	if (mPhysWorld)
	{
		delete mPhysWorld;
	}
	if (mAudioSystem)
	{
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
	}
}
