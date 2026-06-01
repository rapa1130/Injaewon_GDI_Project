#pragma once
#include"Collider.h"
#include"Utillity.h"
#include<Windows.h>
//#include"MyGame.h"
#include<deque>

class MyGame;
class GameObjectBase;

namespace learning
{
	struct ColliderCircle;
	struct ColliderBox;
}

enum class ObjectType
{
	PLAYER,
	ENEMY,
	BULLET,
	ITEM,
	BACKGROUND,
};

struct TraceFrame
{
	GameObjectBase* objectState = nullptr;
	float timeScale = 1.0f;

	TraceFrame(GameObjectBase* pState, float timeScale)
		:objectState(pState), timeScale(timeScale)
	{}
	~TraceFrame()
	{

	}
	void Release();

};

class GameObjectBase
{
	using Vector2f = learning::Vector2f;
	using ColliderBox = learning::ColliderBox;
	using ColliderCircle = learning::ColliderCircle;
	using Collider = learning::Collider;

public:
	GameObjectBase() = delete;
	
	GameObjectBase(MyGame* myGame, ObjectType type) 
		: m_type(type), m_myGame(myGame){
	}


	virtual ~GameObjectBase() = default;

	virtual void Update(float deltaTime) = 0;
	virtual void Render(HDC hdc) = 0;
	virtual void OnCollision(GameObjectBase* other) = 0;
	
	virtual void StoreTrace() = 0;
	virtual void RestoreTrace() = 0;
	virtual void RemoveOldTrace() = 0;
	virtual void RemoveRecentTrace() = 0;


	void SetPosition(float x, float y) { m_pos = { x, y }; }
	void SetDirection(Vector2f dir) { m_dir = dir; }
	void SetSpeed(float speed) { m_speed = speed; }
	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetColliderColor(COLORREF color) { m_colliderSolidColor = color; }

	Vector2f GetPosition() const { return m_pos; }
	Vector2f GetDirection() const { return m_dir; }
	float GetSpeed() const { return m_speed; }
	ObjectType GetType() const { return m_type; }
	bool GetActive() const { return m_bActive; }
	Collider* GetCollider() const { return m_pCollider; }


	void SetWidth(int width) { m_width = width; }
	void SetHeight(int height) { m_height = height; }


protected:
	void Move(float deltaTime)
	{
		m_pos.x += m_dir.x * m_speed * deltaTime;
		m_pos.y += m_dir.y * m_speed * deltaTime;
	}

	GameObjectBase(const GameObjectBase&);

protected:
	ObjectType m_type;
	MyGame* m_myGame;
	bool m_bActive = true;

	int m_width = 0;
	int m_height = 0;

	Vector2f m_pos = { 0.0f, 0.0f };
	Vector2f m_dir = { 0.0f, 0.0f }; // 방향 (단위 벡터)
	float m_speed = 0.0f; // 속력

	Collider* m_pCollider = nullptr;
	COLORREF m_colliderSolidColor = RGB(0, 0, 0);
	COLORREF m_colliderLineColor = RGB(0, 0, 0);

	std::deque<TraceFrame> m_trace;
};
