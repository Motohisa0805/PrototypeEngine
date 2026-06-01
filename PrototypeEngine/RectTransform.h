#pragma once
#include "BaseTransform.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Matrix4;
class UIActorObject;

class RectTransform : public BaseTransform
{
protected:
	Matrix4								mDrawTransform;

	float								mRectScaleWidth;
	float								mRectScaleHeight;

	float								mOffsetX;
	float								mOffsetY;

	//親オブジェクト
	UIActorObject*						mParentActor;
	//子オブジェクトの配列
	vector<UIActorObject*>				mChildActor;

	// 親子関係関数
	void								AddChild(UIActorObject* child);
	void								RemoveChild(UIActorObject* child);
public:
										RectTransform(class UIActorObject* owner);

										~RectTransform();

    const Matrix4&						GetDrawTransform() const { return mDrawTransform; }

	float								GetRectScaleWidth() { return mRectScaleWidth; }
	float								GetRectScaleHeight() { return mRectScaleHeight; }
	void								SetScaleWidthAndHeight(float width, float height);
	void								SetScaleWidth(float width);
	void								SetScaleHeight(float height);

	float								GetOffsetX() { return mOffsetX;}
	float								GetOffsetY() { return mOffsetY;}

	void								SetOffsetX(float x);
	void								SetOffsetY(float y);

	void								ComputeWorldTransform()override;
	//***子オブジェクト関係の処理***
	virtual UIActorObject*				GetParentActor() { return mParentActor; }

	virtual const UIActorObject*		GetChildActor(UIActorObject* actor);

	const vector<UIActorObject*>		GetChildActorList()const { return mChildActor; }
	vector<UIActorObject*>&				GetChildActorListMutable() { return mChildActor; }

	//子オブジェクトを追加
	virtual void						AddChildActor(UIActorObject* childtransform);

	virtual void						RemoveChildActor(UIActorObject* childtransform);


	virtual void						AddParentActor(UIActorObject* parent);
	virtual void						SetParent(UIActorObject* newParent);

	virtual void						RemoveParentActor();

	//子オブジェクトの座標更新
	void								SetDirty()override;
	void								ActiveDirty()override;

	void								Serialize(json& j) const override;
	void								Deserialize(const json& j)override;

	void								DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;
};

