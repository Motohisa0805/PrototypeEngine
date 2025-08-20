#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ActorObject* owner)
	: Component(owner)
	, mVisible(true)
	, mIsAlphaFade(false)
	, mIsLoop(false)
	, mIsDestroyed(false)
	, mParticleLifeTime(1.0f) // デフォルトのパーティクルライフタイム
	, mParticleAllLifeTime(1.0f) // デフォルトの最大パーティクルライフタイム
	, mMaxParticleCount(50) // デフォルトの最大パーティクル数
	, mParticleEmitSpeed(Vector3(1.0f, 2.0f, 1.0f)) // デフォルトのパーティクル発射速度
	, mParticleMaxSize(1.0f) // デフォルトのパーティクルサイズ
	, mParticleMinSize(0.1f) // デフォルトの最小パーティクルサイズ
	, mEmitInterval(0.02f) // 50個/秒
	, mEmitTimer(0.0f)
	, mDefaultColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)) // デフォルトの色（白）
	, mParticleTexture(nullptr) // 初期化時はテクスチャなし
{
	GameWinMain::GetRenderer()->AddParticleComp(this);

	mParticleAllLifeTime = mParticleLifeTime;

	//生成時に初期画像を読み込む
	mParticleTexture = GameWinMain::GetRenderer()->GetTexture("Assets/Particle/Default.png");
}

ParticleSystem::~ParticleSystem()
{
	GameWinMain::GetRenderer()->RemoveParticleComp(this);

	for (auto& p : mParticle)
		delete p;
	mParticle.clear();
}

void ParticleSystem::LoadTexture(string name)
{
	string path = File_P::AssetPath + "Particle/" + name;
	mParticleTexture = GameWinMain::GetRenderer()->GetTexture(path);
	if (!mParticleTexture)
	{
		SDL_Log("Failed to load particle texture: %s", path.c_str());
		return;
	}
}

Vector3 ParticleSystem::AddVelocity()
{
	Vector3 velocity = Vector3(1.0f,1.0f,1.0f);

	// Emit関数の中
	velocity = Random::GetVector(
		Vector3(-0.2f, 1.5f, -0.2f), // 最小値：Yに強い上昇
		Vector3(0.2f, 2.5f, 0.2f)    // 最大値：揺らぎを出す
	) * Random::GetFloatRange(0.8f, 1.2f); // ばらつき

	return velocity;
}

void ParticleSystem::Emit()
{
	ParticleStruct* p = GetInactiveParticleOrCreateNew(); // 再利用 or 新規作成
	if (!p) return; // 最大数ならEmitしない
	p->mActive = true;
	p->mLifetime = p->mMaxLifetime = Random::GetFloatRange(0.5f, 1.0f);
	p->mPosition = mOwner->GetLocalPosition(); // ← 毎回初期位置から
	p->mVelocity = AddVelocity();
	p->mSize = Random::GetFloatRange(mParticleMinSize, mParticleMaxSize);
	p->mColor = mDefaultColor;
}

void ParticleSystem::SetParticleSpeed(Vector3 speed)
{
	mParticleEmitSpeed = speed;
}

void ParticleSystem::SetColor(Vector4 color)
{
	mDefaultColor = color; // 新規パーティクルに反映されるように
	for (auto& p : mParticle)
	{
		if (p->mActive)
			p->mColor = color;
	}
}

ParticleStruct* ParticleSystem::GetInactiveParticleOrCreateNew()
{
	if (mParticle.size() >= mMaxParticleCount)
		return nullptr;

	for (auto& p : mParticle)
	{
		if (!p->mActive)
		{
			return p; // 非アクティブなパーティクルを再利用
		}
	}
	// すべてアクティブなら新規作成して追加
	ParticleStruct* newParticle = new ParticleStruct();
	mParticle.push_back(newParticle);
	return newParticle;
}

void ParticleSystem::SetEmitInterval(float interval)
{
	mEmitInterval = interval;
	mEmitTimer = 0.0f; // タイマーをリセット
}

void ParticleSystem::Update(float deltaTime)
{
	// 全体ライフタイムが0以下なら更新しない
	if (mParticleAllLifeTime <= 0.0f) { return; } 
	mParticleAllLifeTime -= deltaTime;
	mEmitTimer += deltaTime;

	// 一定間隔で1粒ずつ Emit
	while (mEmitTimer >= mEmitInterval)
	{
		Emit();
		mEmitTimer -= mEmitInterval;
	}

	// 各パーティクル更新
	for (auto& p : mParticle)
	{
		if (!p->mActive) continue;

		p->mLifetime -= deltaTime;
		if (p->mLifetime <= 0.0f)
		{
			p->mActive = false;
			continue;
		}

		// 移動
		p->mPosition += p->mVelocity * deltaTime;
	}
	// パーティクルの全体ライフタイムが0以下なら、ループしない場合は破棄
	if( mParticleAllLifeTime <= 0.0f)
	{
		if(!mIsLoop && mIsDestroyed)
		{
			mOwner->SetState(ActorObject::EDead);
		}
		else
		{
			mParticleAllLifeTime = mParticleLifeTime;
		}
	}
}

void ParticleSystem::Draw(Shader* shader)
{
	// カメラのビュー行列の逆行列を取得（ワールド変換行列）
	Matrix4 viewMatrix = GameWinMain::GetRenderer()->GetView();
	viewMatrix.Invert(); // カメラのワールド空間情報

	// カメラの向きを取得
	Vector3 camRight = viewMatrix.GetXAxis(); // X方向（右）
	Vector3 camUp = viewMatrix.GetYAxis(); // Y方向（上）
	Vector3 camForward = viewMatrix.GetZAxis(); // Z方向（前）

	for(int i = 0; i < mParticle.size(); ++i)
	{
		if (mParticle[i]->mActive)
		{
			ParticleStruct* p = mParticle[i];

			// モデル行列
			// === ビルボード用ワールド行列を作成 ===
			Matrix4 world = Matrix4::Billboard(
				p->mPosition, 
				p->mSize, 
				camRight, 
				camUp, 
				camForward
			);
			// ここでワールド行列を設定
			shader->SetMatrixUniform("uWorldTransform", world);
			shader->SetVector4Uniform("uTexUV", Vector4(0.0f, 0.0f, 1.0f, 1.0f));
			// ここでパーティクルの色を設定
			// mColorは元の色で、アルファフェードが有効な場合は透明度を調整
			Vector4 drawColor = p->mColor;
			if (mIsAlphaFade)
			{
				float alpha = p->mLifetime / p->mMaxLifetime;
				drawColor.w *= alpha; // 元の mColor.w に影響しない
			}
			shader->SetVector4Uniform("uColor", drawColor);

			//画像をテクスチャとして使用する場合は、ここでバインドします
			if (!mParticleTexture) return;
			mParticleTexture->SetActive();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
	}
}
