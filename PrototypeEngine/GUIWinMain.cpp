#include "GUIWinMain.h"
#include "Renderer.h"
#include "SceneViewEditor.h"

bool GUIWinMain::mToggle = false;

bool GUIWinMain::isPaused = false;

bool GUIWinMain::isPlaying = false;

Texture* GUIWinMain::mPlayButtonTexture = nullptr;
Texture* GUIWinMain::mPauseButtonTexture = nullptr;
Texture* GUIWinMain::mStopButtonTexture = nullptr;

Renderer* GUIWinMain::mRenderer = nullptr;

bool GUIWinMain::InitializeImGui(SDL_Window* window, SDL_GLContext glContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");


	mPlayButtonTexture = new Texture();
	if (!mPlayButtonTexture->Load("Assets/Editor/PlayButton.png"))
	{
		Debug::ErrorLog("Failed to load play button texture");
		return false;
	}

	mPauseButtonTexture = new Texture();
	if (!mPauseButtonTexture->Load("Assets/Editor/PauseButton.png"))
	{
		Debug::ErrorLog("Failed to load pause button texture");
		return false;
	}

	mStopButtonTexture = new Texture();
	if (!mStopButtonTexture->Load("Assets/Editor/StopButton.png"))
	{
		Debug::ErrorLog("Failed to load stop button texture");
		return false;
	}

	return true;
}

void GUIWinMain::UpdateImGuiState()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	//  デモウィンドウの描画
	ImGui::ShowDemoWindow();
}

void GUIWinMain::RenderImGui()
{
	// ここでImGuiの描画を行う
	// 画面サイズ（SDLで取得したウィンドウ幅/高さ）
	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();
	//再生/一時停止/停止ボタンの状態を管理
	{

		// 上部バーの高さ
		int toolbarHeight = 25;

		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)toolbarHeight));

		ImGui::Begin("Toolbar",
			nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

		ImGui::SetCursorPosX(windowWidth * 0.5f - 30); // 中央寄せ調整（60はボタン群の半幅）

		// ▶ Play
		if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)mPlayButtonTexture->GetTextureID(), ImVec2(15, 15)))
		{
			isPlaying = true;
			isPaused = false;
		}

		// 同じ行に Pause
		ImGui::SameLine();
		if (ImGui::ImageButton("PauseButton", (ImTextureID)(intptr_t)mPauseButtonTexture->GetTextureID(), ImVec2(15, 15)))
		{
			if (isPlaying) isPaused = !isPaused;
		}

		// 同じ行に Stop
		ImGui::SameLine();
		if (ImGui::ImageButton("StopButton", (ImTextureID)(intptr_t)mStopButtonTexture->GetTextureID(), ImVec2(15, 15)))
		{
			isPlaying = false;
			isPaused = false;
		}

		ImGui::End();
	}
	//X = 0の位置にウインドウを配置
	//Xサイズ windowWidth * 0.5f
	{

		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(0.0f, 30));
		ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.5f, (float)windowHeight * 0.5f));
		ImGui::Begin("Scene");
		{
			// ウィンドウサイズに合わせて描画
			ImVec2 size = ImGui::GetContentRegionAvail();
			ImGui::Image(
				(ImTextureID)(intptr_t)mRenderer->GetSceneViewEditor()->GetSceneColorTex(),
				size,
				ImVec2(0, 1),  // uv0 (上下反転に注意)
				ImVec2(1, 0)   // uv1
			);
		}
		ImGui::End();
	}
	{

		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(0.0f, (float)windowHeight * 0.5f));
		ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.5f, (float)windowHeight * 0.5f));
		ImGui::Begin("Game");
		{
			ImGui::Text("GameRenderer");
		}
		ImGui::End();
	}
	//X = windowWidth * 0.5fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.15f
	//Hierarchyウィンドウの描画
	{

		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2((windowWidth * 0.5f), 30));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.15f, (float)windowHeight - 25));
		//  新しいウィンドウの作成
		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			//  テキストの表示
			ImGui::Text("Hello, world!!");

			if (ImGui::Button("TestButton"))
			{
				//  ボタンを押されたときの処理
			}

			ImGui::Checkbox("TestToggle", &mToggle);
			if (mToggle)
			{
				//  チェックがついているときの処理
			}
		}
		ImGui::End();
	}
	//X = windowWidth * 0.65fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.15f
	//Projectウィンドウの描画
	{

		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.65f, 30));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.15f, (float)windowHeight - 25));
		//  新しいウィンドウの作成
		ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{

		}
		ImGui::End();
	}
	//X = windowWidth * 0.8fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.2f
	//SelectItemウィンドウの描画
	{
		// ウインドウ位置とサイズを固定
		ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.8f, 30));
		ImGui::SetNextWindowSize(ImVec2((windowWidth * 0.2f), (float)windowHeight - 25));
		//  新しいウィンドウの作成
		ImGui::Begin("SelectItem",nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			//  テキストの表示
			ImGui::Text("Hello, world!!");

			if (ImGui::Button("TestButton"))
			{
				//  ボタンを押されたときの処理
			}

			ImGui::Checkbox("TestToggle", &mToggle);
			if (mToggle)
			{
				//  チェックがついているときの処理
			}
		}
		ImGui::End();
	}



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIWinMain::ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (mPlayButtonTexture)
	{
		mPlayButtonTexture->Unload();
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	if (mPauseButtonTexture)
	{
		mPauseButtonTexture->Unload();
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	if (mStopButtonTexture)
	{
		mStopButtonTexture->Unload();
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
}
