#pragma once

#include"GameObject.h"
#include<iostream>
#include"MyGame.h"
#include<deque>
#include"Bullet.h"

class EnemyGameObject : public GameObject
{
	using Collider = learning::Collider;

public:
	EnemyGameObject(MyGame* myGame)
		:GameObject(myGame,ObjectType::ENEMY)
	{
		float randFactor = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
		m_initialReloadTime = 2000.0f + randFactor * 500;
		m_nowReloadTime = m_reloadTimeRemaining = m_initialReloadTime;

		m_colliderSolidColor = RGB(0, 0, 0);
		m_colliderLineColor = RGB(0, 0, 0);
	}

	EnemyGameObject(const EnemyGameObject& other)
		:GameObject(other)
	{
		m_initialReloadTime = other.m_initialReloadTime;
		m_reloadTimeRemaining = other.m_reloadTimeRemaining;
		m_nowReloadTime = other.m_nowReloadTime;
	}
	virtual void OnCollision(GameObjectBase* other)
	{
		__super::OnCollision(other);

		switch (other->GetType())
		{
		case ObjectType::BULLET:
		{
			BulletGameObject* pBGO = dynamic_cast<BulletGameObject*>(other);
			if (pBGO == nullptr) {
				//std::cout << "BulletGameObject ptr is NUll, cannot Çüº¯È¯" << std::endl;
				return;
			}
			if (pBGO->GetShooter()->GetType() == ObjectType::PLAYER)
			{
				m_bActive = false;
				return;
			}
		}
			break;
		}
	}
	virtual void Render(HDC hdc) override
	{
		if (!m_bActive) return;
		if (m_myGame->IsRewinding())
		{
			m_colliderLineColor = m_colliderSolidColor = RGB(255, 255, 255);
		}
		else
		{
			m_colliderLineColor = m_colliderSolidColor = RGB(0, 0, 0);
		}
		DrawCollider(hdc);
	}


	float Lerp(float A, float B, float Alpha)
	{
		return A * (1 - Alpha) + B * Alpha;
	}

	virtual void Update(float deltaTime) override
	{
		using Vector2f = learning::Vector2f;

		__super::Update(deltaTime);

		if (!m_bActive) return;

		m_reloadTimeRemaining -= deltaTime;
		if (m_reloadTimeRemaining < 0.0f)
		{
			GameObject* player = m_myGame->GetPlayer();
			Vector2f targetV2 = player->GetPosition();
			
			float randX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
			float randY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
			Vector2f randV2 = targetV2 + Vector2f(randX, randY) * 300.0f;

			m_myGame->CreateBullet(this, randV2);

			if (m_nowReloadTime > m_initialReloadTime * 0.1f) m_nowReloadTime *= 0.95f;
			m_reloadTimeRemaining = m_nowReloadTime;
		}
		else
		{

		}
	}

	virtual void StoreTrace() override
	{
		GameObjectBase* traceGOB = new EnemyGameObject(*this);
		const TraceFrame& traceFrm = TraceFrame(traceGOB, m_myGame->GetGameTimeScale());
		m_trace.push_back(traceFrm);
	}

	virtual void RestoreTrace() override
	{
		if (m_trace.empty()) 	return;

		__super::RestoreTrace();

		GameObjectBase* pStateGO = m_trace.back().objectState;
		EnemyGameObject* pStateEGO = dynamic_cast<EnemyGameObject*>(pStateGO);
		
		if (pStateEGO == nullptr)
		{
			//std::cout << "Fail to Casting EnemyGameObject" << std::endl;
			return;
		}

		float ts = m_trace.back().timeScale;
		float fixedTs = m_myGame->GetFixedTimeScale();
		int iteration = (int)(fixedTs / ts);
		for (int i = 0; i < iteration; i++)
		{
			m_reloadTimeRemaining = pStateEGO->m_reloadTimeRemaining;
			m_nowReloadTime = pStateEGO->m_nowReloadTime;
		}

	}
private:
	float m_initialReloadTime;
	float m_reloadTimeRemaining;
	float m_nowReloadTime;
};