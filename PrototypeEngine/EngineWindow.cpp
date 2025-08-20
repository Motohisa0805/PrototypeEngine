#include "EngineWindow.h"

EngineState EngineWindow::mEngineState = EngineState::None;

EngineWindow::EngineWindow()
	: mRenderer(nullptr)
{
	GUIWinMain::ShutdownImGui();
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
	/*
	// Renderer�̐���
	mRenderer = new Renderer();
	if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()))
	{
		Debug::ErrorLog("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}
	*/

	// SDL_ttf�̏�����
	if (!TTF_Init())
	{
		Debug::ErrorLog("Failed to initialize SDL_ttf");
		return false;
	}

	Time::InitializeDeltaTime();

	EngineWindow::mEngineState = EngineState::Run;

	//  ImGui�̏���������
	GUIWinMain::InitializeImGui(mRenderer->GetWindow(), mRenderer->GetContext());
	
	//�N�����ɍŏ��̃V�[����������
	SceneManager::InitializeScenes();

	mGameWindow = new GameWinMain();
	mGameWindow->Initialize();
	
	return true;
}

//�G���W���v���W�F�N�g�̃��[�v����
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::Run)
	{
		Time::UpdateDeltaTime();

		GUIWinMain::UpdateImGuiState();

		EngineRender();

		//��������Q�[�����̍X�V�J�n
		if (GUIWinMain::IsPlaying)
		{

			mGameWindow->RunLoop();
		}

	}
}

void EngineWindow::EngineRender()
{
	//ImGui�̕`��
	{
		GUIWinMain::RenderImGui();
	}
	{
		
	}
}

void EngineWindow::EngineUnloadData()
{
}

void EngineWindow::EngineShutdown()
{
	EngineUnloadData();
	TTF_Quit();
	if (mRenderer)
	{
		mRenderer->Shutdown();
		delete mRenderer;
	}
	SDL_Quit();
}
