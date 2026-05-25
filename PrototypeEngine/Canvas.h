#pragma once
#include "Math.h"
#include "Typedefs.h"
#include "UIActor.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/
class Texture;
class Shader;
class Font;
class Image;
class Text;
class Button;

//ポーズ、ダイアログなどのUIの基底クラス
// UnityのCanvasに近いクラス
class Canvas : public UIActorObject
{
protected:

public:
	Canvas(uint64_t id = 0);
	virtual ~Canvas();

	// シーンから呼ばれる関数群をオーバーライド
	void Update(float deltaTime) override;
};