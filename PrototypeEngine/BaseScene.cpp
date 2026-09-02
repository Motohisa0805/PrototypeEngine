#include "BaseScene.h"
#include "AudioSystem.h"
#include "BaseCamera.h"
#include "DirectionalLightComponent.h"
#include "EditorSettingsManager.h"
#include "EngineWindow.h"
#include "FreeCamera.h"
#include "GameFunctions.h"
#include "Renderer.h"
#include "SceneSerializer.h"
#include "VertexArray.h"

BaseScene::BaseScene()
    : mActorManager(nullptr)
    , mUIActorManager(nullptr)
    , mAudioSystem(nullptr)
    , mFixed_Delta_Time(0.02f)
    , mPlayer(nullptr)
    , mCameraMap()
    , mFixedTimeAccumulator(0.0f)
    , mName("BaseScene")
    , mNextActorID(0)
    , mIsDirtyFlag(false)
{
    if (mActorManager == nullptr)
    {
        mActorManager = new ActorManager();
    }

    if (mUIActorManager == nullptr)
    {
        mUIActorManager = new UIActorManager();
    }
}

void BaseScene::LoadSkyBoxTexture(const string& file)
{
    // 読み込みが成功なら
    if (EngineWindow::GetRenderer()->GetSkyBoxRenderer()->Load(file))
    {
        mLoadSkyBoxTexturePath = file;
    }
}

bool BaseScene::Initialize()
{
    //-----------------------------------------------------
    // ここでシーン内のオブジェクトは全て生成されている。
    // もし読み込んだシーンにカメラ、環境光がないなら生成
    FreeCamera*                mainCam  = nullptr;
    DirectionalLightComponent* dirLight = nullptr;
    // シーンにあるか調べる
    for (auto* actor : mActorManager->GetActors())
    {
        if (!mainCam)
        {
            mainCam = actor->GetComponent<FreeCamera>();
        }
        if (!dirLight)
        {
            dirLight = actor->GetComponent<DirectionalLightComponent>();
        }
        // 2つともあったらfor文を抜ける
        if (mainCam && dirLight)
            break;
    }
    if (!mainCam)
    {
        ActorObject* cameraActor = new ActorObject();
        cameraActor->SetName("MainCamera");
        // 初期位置と回転を設定
        cameraActor->GetTransform()->SetPosition(Vector3(0.0f, 2.0f, -5.0f));
        cameraActor->GetTransform()->SetRotation(
            Quaternion(Vector3::UnitY, 0.0f));

        // ここはカメラコンポーネントを改造して変更予定
        // FreeCameraコンポーネントをアタッチ
        FreeCamera* freeCamComp = new FreeCamera(cameraActor);
        freeCamComp->SetIsMain(true); // メインカメラに設定
        cameraActor->AddComponent(freeCamComp);
    }
    // LightActorがないなら
    if (!dirLight)
    {
        ActorObject* lightActor = new ActorObject();
        lightActor->SetName("Directional Light");
        // 太陽光のデフォルト回転 (例: X軸で-45度回転、Y軸で45度回転)
        // 90度：X→Yに向く → Y成分 = 1（昼！）
        Quaternion rot =
            Quaternion::CreateFromAxisAngle(Vector3::UnitZ, -45.0f);
        lightActor->GetTransform()->SetLocalRotation(rot);

        // DirectionalLightComponent をアタッチ
        lightActor->AddComponent(new DirectionalLightComponent(lightActor));
    }
    //-----------------------------------------------------

    Font* font = GetFont("NotoSansJP-Bold.ttf");

    return true;
}

bool BaseScene::InputUpdate(const InputState& state)
{

    if (GameStateClass::gGameState == GameState::GamePlay)
    {
        for (auto actor : mActorManager->GetActors())
        {
            if (actor->GetState() == ActorObject::EActive)
            {
                actor->ProcessInput(state);
            }
        }
        for (auto actor : mUIActorManager->GetActors())
        {
            if (actor->GetState() == ActorObject::EActive)
            {
                actor->ProcessInput(state);
            }
        }
    }

    return true;
}

bool BaseScene::FixedUpdate()
{
    float deltaTime = Time::gDeltaTime;
    mFixedTimeAccumulator += deltaTime;

    // 複数回 FixedUpdate が必要な場合に備える
    while (mFixedTimeAccumulator >= mFixed_Delta_Time)
    {
        // Rigidbody などの物理処理をここで呼ぶ
        mActorManager->FixedUpdateActors(deltaTime);

        EngineWindow::GetPhysWorld()->SweepAndPruneXYZ(deltaTime);

        // UIの固定更新
        mUIActorManager->FixedUpdateActors(deltaTime);

        mFixedTimeAccumulator -= mFixed_Delta_Time;
    }

    return true;
}

bool BaseScene::Update()
{
    // 特定のシーンに読み込まれたオブジェクトやコンポーネントを
    //  まとめて処理する部分
    float deltaTime = Time::gDeltaTime;
    // Update all actors
    mActorManager->UpdateActors(deltaTime);

    // Update audio system
    mAudioSystem->Update(deltaTime);

    // 全UIアクターの更新
    mUIActorManager->UpdateActors(deltaTime);

    return true;
}

bool BaseScene::EditorUpdate(bool isRun)
{
    // オブジェクトの座標が更新された時だけ
    if (!mIsDirtyFlag)
    {
        return false;
    }
    float deltaTime = Time::gDeltaTime;
    // 特定のシーンに読み込まれたオブジェクトやコンポーネントを
    //  まとめて処理する部分
    //  Update all actors
    mActorManager->UpdateActors(deltaTime);

    // 全UIアクターの更新
    mUIActorManager->UpdateActors(deltaTime);
    /*
    //実行中じゃなければ
    if (!isRun)
    {
        //編集での変更があればそれを記録する
        string startupScenePath =
    EditorSettingsManager::GetInstance().GetLastOpenedScene();
        SceneSerializer::WriteEditingSceneData(startupScenePath,this);
        EditorSettingsManager::SetSaveFlag(true);
    }
    */
    mIsDirtyFlag = false;
    return true;
}

void BaseScene::ClearDirtyFlag() { mIsDirtyFlag = false; }

Font* BaseScene::GetFont(const string& fileName)
{
    string filePath = FontFile::FontFilePath + fileName;
    auto   iter     = mFontMap.find(filePath);
    if (iter != mFontMap.end())
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
    mFontMap.emplace(filePath, font);
    return font;
}

SkeletonData* BaseScene::GetSkeleton(const string& fileName)
{
    string file = fileName;
    auto   iter = mSkeletonMap.find(file);
    // すでにあるならそれを使う
    if (iter != mSkeletonMap.end())
    {
        return iter->second;
    }
    // 新しく読み込み
    else
    {
        SkeletonData* sk = new SkeletonData();
        if (sk->LoadFromSkeletonBin(file))
        {
            mSkeletonMap.emplace(file, sk);
        }
        else if (sk->Load(file))
        {
            mSkeletonMap.emplace(file, sk);
        }
        else
        {
            delete sk;
            sk = nullptr;
        }
        return sk;
    }
    // 読み込み失敗
    return nullptr;
}

void BaseScene::AddCamera(BaseCamera* camera)
{
    // すでに登録されている場合は何もしない
    for (auto& cam : mCameraMap)
    {
        if (cam.second == camera)
        {
            return;
        }
    }
    // メインカメラに設定
    for (auto& cam : mCameraMap)
    {
        cam.second->SetIsMain(false);
    }
    camera->SetIsMain(true);
    // 名前を自動生成して登録
    int    index = mCameraMap.size();
    string name  = "Camera" + std::to_string(index);
    mCameraMap.emplace(name, camera);
}

void BaseScene::RemoveCamera(BaseCamera* camera)
{
    for (auto iter = mCameraMap.begin(); iter != mCameraMap.end(); ++iter)
    {
        if (iter->second == camera)
        {
            mCameraMap.erase(iter);
            break;
        }
    }
}

BaseCamera* BaseScene::GetCamera(const string& name)
{
    return mCameraMap[name];
}

int BaseScene::GetSceneAllVertices()
{
    int vertices = 0;

    for (auto* actor : mActorManager->GetActors())
    {
        MeshRenderer* comp = actor->GetComponent<MeshRenderer>();
        if (comp)
        {
            for (auto* mesh : comp->GetMeshs())
            {
                for (auto* vertArray : mesh->GetVertexArrays())
                {
                    vertices += vertArray->GetNumVerts();
                }
            }
        }
    }

    return vertices;
}

void BaseScene::UnloadData()
{
    // シーン内のデータを全解放

    // Delete actors
    // Because ~Actor calls RemoveActor, have to use a different style loop
    if (mActorManager)
    {
        mActorManager->UnloadActors();
        delete mActorManager;
        mActorManager = nullptr;
    }

    if (mUIActorManager)
    {
        mUIActorManager->UnloadActors();
        delete mUIActorManager;
        mUIActorManager = nullptr;
    }

    // Unload fonts
    for (auto& f : mFontMap)
    {
        if (f.second)
        {
            f.second->Unload();
            delete f.second;
            f.second = nullptr;
        }
    }
    mFontMap.clear();

    // Unload skeletons
    for (auto s : mSkeletonMap)
    {
        if (s.second)
        {
            delete s.second;
            s.second = nullptr;
        }
    }
    mSkeletonMap.clear();

    for (auto c : mCameraMap)
    {
        if (c.second)
        {
            delete c.second;
            c.second = nullptr;
        }
    }
    mCameraMap.clear();

    if (mAudioSystem)
    {
        mAudioSystem->Shutdown();
        delete mAudioSystem;
        mAudioSystem = nullptr;
    }
}

EditorScene::EditorScene()
{
    if (!InputSystem::Initialize())
    {
        SDL_Log("Failed to initialize input system");
    }

    // Audio Systemを作成
    mAudioSystem = new AudioSystem(this);
    if (!mAudioSystem->Initialize())
    {
        SDL_Log("Failed to initialize audio system");
        mAudioSystem->Shutdown();
        delete mAudioSystem;
        mAudioSystem = nullptr;
    }
}

void EditorScene::EditorInitilaize() {}

void EditorScene::SetName(const string& name) { mName = name; }
