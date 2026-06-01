#pragma once
#include"GameObjectBase.h"
#include"GameObject.h"
#include<iostream>


class GameObject : public GameObjectBase
{
	using ColliderCircle = learning::ColliderCircle;
	using ColliderBox = learning::ColliderBox;
public:
	/*GameObject(const GameObject&) = delete;*/
	GameObject(MyGame* myGame,ObjectType type) : GameObjectBase(myGame,type){
	}
	virtual ~GameObject() override;

	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;
	virtual void OnCollision(GameObjectBase* other) override;

	virtual void StoreTrace() override;
	virtual void RestoreTrace() override;
	virtual void RemoveOldTrace() override;
	virtual void RemoveRecentTrace() override;

	void SetColliderCircle(float radius);
	void SetColliderBox(float halfWidth, float halfHeight);

protected:
	void DrawCollider(HDC hdc);
	void Move(float deltaTime);

	//void DrawBitmap(HDC hdc);
	GameObject(const GameObject&);
private:
	COLORREF m_enemyColor = RGB(255, 255, 0);
	COLORREF m_playerColor = RGB(0, 0, 255);
	//GameObject(GameObject*);

};