#include "EngineWindow.h"
#include "BaseScene.h"

EngineState EngineWindow::mEngineState = EngineState::Run;

Renderer* EngineWindow::mRenderer = nullptr;

EngineWindow::EngineWindow()
{
}

EngineWindow::~EngineWindow()
{
	
}

bool EngineWindow::EngineInitialize()
{
	// SDL�̏�����
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult < 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	// Renderer�̐���
	mRenderer = new Renderer();
	if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()))
	{
		Debug::ErrorLog("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	// SDL_ttf�̏�����
	if (!TTF_Init())
	{
		Debug::ErrorLog("Failed to initialize SDL_ttf");
		return false;
	}

	Time::InitializeDeltaTime();

	EngineWindow::mEngineState = EngineState::Run;

	

	mGameWindow = new GameWinMain();
	mGameWindow->Initialize();
	
	//�N�����ɍŏ��̃V�[����������
	SceneManager::InitializeScenes();
	//  ImGui�̏���������
	GUIWinMain::InitializeImGui(mRenderer->GetWindow(), mRenderer->GetContext());
	//���ň��X�V���s��
	mGameWindow->GameRunLoop();
	return true;
}

void EngineWindow::EngineProcessInput()
{
	const InputState& state = InputSystem::GetState();
	InputSystem::Update();

	//���͑���
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// ImGui�p�̃C�x���g����
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type)
		{
			//���s���I�������true
		case SDL_EVENT_QUIT:
			if (!GUIWinMain::IsPlaying())
			{
				mEngineState = EngineState::End;
			}
			break;
		}
	}

	if (GUIWinMain::IsPlaying())
	{
		//�Q�[�������s���Ȃ�
		mGameWindow->InputUpdate();
	}

	InputSystem::PrepareForUpdate();
}

//�G���W���v���W�F�N�g�̃��[�v����
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::End)
	{
		Time::UpdateDeltaTime();

		EngineProcessInput();

		GUIWinMain::UpdateImGuiState();


		//��������Q�[�����̍X�V�J�n
		//�Q�[�����J�n������
		if (GUIWinMain::IsPlaying())
		{
			//�J�n�����u�ԂȂ�
			if (GUIWinMain::IsStarting())
			{
				//Renderer�̂��̂��A�����[�h
				mRenderer->UnloadData();
				//���݂̃V�[���̃I�u�W�F�N�g�A�摜�Ȃǂ��A�����[�h
				SceneManager::GetNowScene()->UnloadData();
				GameStateClass::SetGameState(GameState::GamePlay);
				//�V�����V�[���̏�����
				SceneManager::GetNowScene()->Initialize();
				//Renderer�̃V�[�����ύX
				mRenderer->SetBaseScene(SceneManager::GetNowScene());

				GUIWinMain::SetIsStarting(false);
			}
			mGameWindow->GameRunLoop();
		}


		//�I���{�^���������ꂽ��
		if( GUIWinMain::IsPushEnd() )
		{
			//���݂̃V�[���̃I�u�W�F�N�g�A�摜�Ȃǂ��A�����[�h
			SceneManager::GetNowScene()->UnloadData();
			//Renderer�̂��̂��A�����[�h
			mRenderer->UnloadData();
			GameStateClass::SetGameState(GameState::GameEnd);
			SceneManager::ReleaseAllScenes();
			SceneManager::InitializeScenes();
		    //���ň��X�V���s��
			mGameWindow->GameRunLoop();

			GUIWinMain::SetIsPushEnd(false);
		}
		EngineRender();

	}
}

void EngineWindow::EngineRender()
{
	mRenderer->StartDraw();
	//ImGui�̕`��
	GUIWinMain::RenderImGui();
	mRenderer->EndDraw();
}

void EngineWindow::EngineUnloadData()
{

	if(mGameWindow)
	{
		mGameWindow->Shutdown();
		delete mGameWindow;
		mGameWindow = nullptr;
	}
	SceneManager::ReleaseAllScenes();
}

void EngineWindow::EngineShutdown()
{
	if (mGameWindow)
	{
		mGameWindow->Shutdown();
		delete mGameWindow;
		mGameWindow = nullptr;
	}
	SceneManager::ReleaseAllScenes();
	TTF_Quit();
	if (mRenderer)
	{
		mRenderer->UnloadData();
		mRenderer->Shutdown();
		delete mRenderer;
		mRenderer = nullptr;
	}
	GUIWinMain::ShutdownImGui();
	SDL_Quit();
}
