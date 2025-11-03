#pragma once
#include "PrototypeEngine_API.h"
#include <cstdint>
#include "Math.h"
#include "Typedefs.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/
class ActorObject;
class BaseScene;

//リフレクションシステム定義
enum class EPropertyType
{
	E_PT_NONE,
	E_PT_FLOAT,
	E_PT_VECTOR3,
	E_PT_COLOR3,
	E_PT_INT,
	E_PT_BOOL,
	E_PT_STRING,
	//必要に追加
};

//プロパティ情報構造体
struct PropertyInfo
{
	string			sName;		// インスペクターに表示する名前
	EPropertyType	sType;		// プロパティの型
	size_t			sOffset;	// Componentオブジェクトの先頭からのメンバ変数のオフセット
	string			sGetterName;// シリアライズ・デシリアライズ用
	string			sSetterName;
};

// --------------------------------------------------
// GUI描画簡略化マクロ (派生クラスの DrawCustomGUI() 内で使用)
// --------------------------------------------------
//【リフレクションマクロ】
//ヘッダーファイルで使うマクロ
#define REFLECT_CLASS_BEGIN(Class, BaseClass) \
public: \
	using self = Class; \
    using Base = BaseClass; \
    static const std::vector<PropertyInfo>& GetPropertiesStatic() { \
        static std::vector<PropertyInfo> sProperties; \
        if (sProperties.empty()) { \
            /* ここで親クラスのプロパティを継承・登録する */ \
            const auto& baseProps = Base::GetPropertiesStatic(); \
            sProperties.insert(sProperties.end(), baseProps.begin(), baseProps.end());

#define REFLECT_PROPERTY(Type, Name, Getter, Setter) \
    do { \
        sProperties.emplace_back(PropertyInfo{#Name, EPropertyType::E_PT_##Type, offsetof(self, Name), #Getter, #Setter}); \
    } while(false)

#define REFLECT_END \
        } \
        return sProperties; \
    } \
    const std::vector<PropertyInfo>& GetProperties() const override { return GetPropertiesStatic(); } \
private:

// REFLECT_BASE_BEGINは使わずに、REFLECT_CLASS_BEGINを使用
#define REFLECT_BASE_BEGIN(Class) REFLECT_CLASS_BEGIN(Class, Component)




//コンポーネントの基底クラス
// Unityのコンポーネントに近いクラス
class PROTOTYPEENGINE_API Component
{
protected:
	// コンポーネント元のオブジェクト
	ActorObject*		mOwner;
	BaseScene*			mGame;
	// コンポーネントの順序を更新する
	int					mUpdateOrder;

	string				mName;

	Vector4				mHeaderColor;
	Vector4				mHeaderHoveredColor;
	Vector4				mHeaderActiveColor;
public:
	// コンストラクタ
	// （更新順序が低いほど、コンポーネントが早く更新される）
	Component(class ActorObject* owner, int updateOrder = 100);
	// デストラクター
	virtual				~Component();

	// --------------------------------------------------
	// 【追加】リフレクション関連の仮想関数と静的関数
	// --------------------------------------------------

	//Componentの共通プロパティ関連の仮想関数と静的関数
	static const vector<PropertyInfo>& GetPropertiesStatic();

	//インスタントのプロパティリストを取得する仮想関数
	virtual const std::vector<PropertyInfo>& GetProperties() const;

	virtual void		FixedUpdate(float deltaTime);
	// デルタ時間でこのコンポーネントを更新してください
	virtual void		Update(float deltaTime);
	// このコンポーネントの入力を処理する
	virtual void		ProcessInput(const struct InputState& keyState) {}
	// 世界の変化によって呼び出される
	virtual void		OnUpdateWorldTransform() {}
	virtual void		InitializeDrawCustomGUI();
	// GUI描画関数
	virtual void		DrawCustomGUI(const std::vector<PropertyInfo>& properties);
	virtual void		EndDrawCustomGUI();

	//DrawCustomGUIないの動的アクセスに使用するポインタ取得マクロ
	//baseポインタとオフセットを使って、メンバ変数へのポインタを計算する
#define GET_MEMBER_PTR(OwnerPtr,Offset,Type)\
		reinterpret_cast<Type*>(reinterpret_cast<char*>(OwnerPtr) + Offset)

	//プロパティ描画用のヘルパー関数
	void				DrawFloatProperty(const PropertyInfo& prop);
	void				DrawIntProperty(const PropertyInfo& prop);
	void				DrawBoolProperty(const PropertyInfo& prop);

	class ActorObject*	GetOwner() { return mOwner; }
	int					GetUpdateOrder() const { return mUpdateOrder; }

	string				GetName()const { return mName; }
	// JSONに変換するメソッド
	virtual void		Serialize(json& j) const;
	// JSONから復元するメソッド
	virtual void		Deserialize(const json& j);

};