#pragma once

#include"GameObject.h"
#include<iostream>

class BulletGameObject : public GameObject
{
public:
	BulletGameObject(MyGame* myGame,GameObjectBase* shooter)
		:GameObject(myGame, ObjectType::BULLET)
	{
		this->m_shooter = shooter;
		m_maxLiveTime = 10000.0f;
	}

	virtual void Render(HDC hdc)
	{
		if (!m_bActive) return;
		if (m_myGame->IsRewinding())
		{
			m_colliderLineColor = m_colliderSolidColor = RGB(255, 255, 255);
		}
		else
		{
			m_colliderLineColor = m_colliderSolidColor = m_bulletColor;

		}
		DrawCollider(hdc);
	}

	void Update(float deltaTime) override
	{
		__super::Update(deltaTime);

		//if (m_myGame->IsGameStopped()) return;
		if (m_myGame->IsRewinding()) m_liveTime -= m_myGame->GetGameTimeScale();
		else m_liveTime += m_myGame->GetGameTimeScale();

		if (m_liveTime > m_maxLiveTime) m_myGame->RegisterToDestroy(this);

		if (m_myGame->GetPlayer() == m_shooter)
		{
			//std::cout << "m_liveTime : " << m_liveTime << std::endl;
		}
	}

	void OnCollision(GameObjectBase* other) override
	{

	}

	virtual void RemoveRecentTrace() override
	{
		__super::RemoveRecentTrace();
		if (m_trace.empty()) m_myGame->RegisterToDestroy(this);
	}
	

	void SetBulletColor(COLORREF color) { m_bulletColor = color; }
	GameObjectBase* GetShooter() const { return m_shooter; }
	float GetLiveTime() const { return m_liveTime; }

private:
	COLORREF m_bulletColor = RGB(0, 0, 0);
	GameObjectBase* m_shooter = nullptr;
	float m_liveTime = 0;
	float m_maxLiveTime = 0;
};