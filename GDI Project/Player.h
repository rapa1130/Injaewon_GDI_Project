#pragma once
#include"GameObject.h"

class GameObjectPlayer : public GameObject
{
public:
	GameObjectPlayer(MyGame* myGame)
		:GameObject(myGame, ObjectType::PLAYER)
	{
		m_colliderSolidColor = RGB(0, 0, 255);
		m_colliderLineColor = RGB(0, 0, 255);
	}
	virtual void OnCollision(GameObjectBase* other) 
	{
		switch (other->GetType())
		{
			case ObjectType::BULLET:
			{
				m_myGame->SetGameStop(true);
			}break;

			case ObjectType::ENEMY:
			{
				m_myGame->SetGameStop(true);
			}break;
		}
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
			m_colliderLineColor = m_colliderSolidColor = RGB(0, 0, 255);
		}
		
		DrawCollider(hdc);
	}
	
	void Update(float deltaTime) override
	{
		using learning::Vector2f;
		using learning::ColliderCircle;
		using learning::Collider;

		__super::Update(deltaTime);
		float width = m_myGame->GetWidth();
		float height = m_myGame->GetHeight();

		ColliderCircle* colCir = dynamic_cast<ColliderCircle*>(m_pCollider);
		if (colCir == nullptr) {
			//std::cout << "Collider Circle is nullptr in Player.h_Update";
			return;
		}

		float radius = colCir->m_radius;

		float xClamped = std::max(std::min(width - radius, m_pos.x), radius);
		float yClamped = std::max(std::min(height - radius, m_pos.y), radius);

		m_pos = Vector2f(xClamped, yClamped);
	}

	virtual void RestoreTrace() override
	{
		if (m_trace.empty()) return;
		__super::RestoreTrace();

		float gameTs = m_trace.back().timeScale;
		float fixedTs = m_myGame->GetFixedTimeScale();
		int iteration = (int)(fixedTs / gameTs);

		GameObjectBase* pStateGOB = m_trace.back().objectState;
		//GameObjectPlayer* pStateGOP = dynamic_cast<GameObjectPlayer*>(pStateGOB);

		//if (pStateGOP == nullptr)
		//{
		//	std::cout << "pStateGOP is Null!" << std::endl;
		//	return;
		//}

	}
};