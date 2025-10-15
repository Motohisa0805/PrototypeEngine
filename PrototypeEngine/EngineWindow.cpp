#include "EngineWindow.h"
#include "BaseScene.h"
#include "SceneEditorCamera.h"
#include "GameViewPanel.h"
#include "ComponentFactory.h"

EngineState EngineWindow::mEngineState = EngineState::Run;

Renderer* EngineWindow::mRenderer = nullptr;

SceneEditorCamera* EngineWindow::mSceneEditorCamera = nullptr;

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

	RegisterAllComponents();

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

	mSceneEditorCamera = new SceneEditorCamera();
	GameStateClass::mDebugFrag = true;
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
		InputSystem::ProcessEvent(event);
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
	//-------------------------------------------------------
	// �Q�[���G���W�����̓��͏���
	//-------------------------------------------------------
	//ESC�L�[�������ăQ�[�����͂�����
	if (state.Keyboard.GetKeyDown(KEY_ESCAPE)||!GUIWinMain::GetGameViewPanel()->IsMouseHovered())
	{
		if (InputContextManager::IsGameInputActive())
		{
			InputContextManager::SetContext(InputContext::Engine);
		}
	}

	// Debug�r���h�̏ꍇ�̏���
	if (state.Keyboard.GetKeyDown(SDL_SCANCODE_F1))
	{
		GameStateClass::mDebugFrag = !GameStateClass::mDebugFrag;
	}
	//�V�[���r���[�̃G�f�B�^�[�J��������
	mSceneEditorCamera->ProcessInput(state);
	//�V�[���̓��͏���
	if (GUIWinMain::IsPlaying()&& !GUIWinMain::IsPaused())
	{
		if (InputContextManager::IsGameInputActive())
		{
			//�Q�[�������s���Ȃ�
			mGameWindow->InputUpdate();
		}
	}

	InputSystem::PrepareForUpdate();
}

//�G���W���v���W�F�N�g�̃��[�v����
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::End)
	{
		//�f���^�^�C���X�V
		Time::UpdateDeltaTime();
		//���͏���
		EngineProcessInput();
		//ImGui�̏�ԍX�V
		GUIWinMain::UpdateImGuiState();
		//�G�f�B�^�[�p�J�����̍X�V
		mSceneEditorCamera->Update();

		//��������Q�[�����̍X�V�J�n
		//�Q�[�����J�n������
		if (GUIWinMain::IsPlaying())
		{
			if (!GUIWinMain::IsPaused())
			{
				//�J�n�����u�ԂȂ�
				if (GUIWinMain::IsStarting())
				{
					GUIWinMain::ResetPointer();
					//Renderer�̂��̂��A�����[�h
					//mRenderer->UnloadData();
					//���݂̃V�[���̃I�u�W�F�N�g�A�摜�Ȃǂ��A�����[�h
					//SceneManager::GetNowScene()->UnloadData();
					GameStateClass::SetGameState(GameState::GamePlay);
					//�V�����V�[���̏�����
					//SceneManager::GetNowScene()->Initialize();
					//Renderer�̃V�[�����ύX
					//mRenderer->SetBaseScene(SceneManager::GetNowScene());

					GUIWinMain::SetIsStarting(false);
				}
				mGameWindow->GameRunLoop();
			}
		}
		else
		{
			mGameWindow->LoadGame_Engine();
			SceneManager::GetNowScene()->EditorUpdate();
		}
		//�I���{�^���������ꂽ��
		if(GUIWinMain::IsPushEnd())
		{
			SceneManager::PlayEndInitilaizeScene();
		    //���ň��X�V���s��
			mGameWindow->GameRunLoop();
			GUIWinMain::SetIsPushEnd(false);
		}
		EngineRender();
	}
}

void EngineWindow::EngineRender()
{
	//Renderer�̕`��J�n
	mRenderer->StartDraw();
	//ImGui�̕`��
	GUIWinMain::RenderImGui();
	//Renderer�̕`��I��
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
	if (mSceneEditorCamera)
	{
		delete mSceneEditorCamera;
		mSceneEditorCamera = nullptr;
	}
	GUIWinMain::ShutdownImGui();
	SDL_Quit();
}
