#include "INC_Windows.h"

#include "Collider.h"
#include "GameObjectBase.h"
#include "GameObject.h"
#include "MyGame.h"

#include <assert.h>
#include<iostream>

GameObject::~GameObject()
{
	for (TraceFrame& trace : m_trace)
	{
		trace.Release();
	}
	m_trace.clear();

	if (m_pCollider)
	{
		delete m_pCollider;
		m_pCollider = nullptr;
	}
}


void GameObject::Update(float deltaTime)
{
	//if (!m_bRewinding && !m_bGameStop)
//{

//}
	if (m_myGame->IsRewinding() || m_myGame->IsGameStopped()) return;

	StoreTrace();
	if (m_bActive) Move(deltaTime);
	if (m_pCollider) m_pCollider->m_center = m_pos;
	RemoveOldTrace();
	// 
	//UpdateFrame(deltaTime);
}

void GameObject::Render(HDC hdc)
{
	if (!m_bActive) return;
	DrawCollider(hdc);
	//DrawBitmap(hdc);
}


GameObjectBase::GameObjectBase(const GameObjectBase& other)
	:m_type(other.m_type), m_myGame(other.m_myGame),
	m_bActive(other.m_bActive), m_width(other.m_width), m_height(other.m_height),
	m_pos(other.m_pos), m_dir(other.m_dir), m_speed(other.m_speed)
{
	m_pCollider = other.m_pCollider->Clone();
}

void GameObject::OnCollision(GameObjectBase* other)
{
	using Collider = learning::Collider;
	using Vector2f = learning::Vector2f;

	Collider* otherCol = other->GetCollider();
	if (otherCol == nullptr) {
		std::cout << "GameObject::OnCollision_ Collider 형변환 실패" << std::endl;
		return;
	}
	Vector2f normalVec2 = (m_pCollider->m_center - otherCol->m_center).Normalized();
	Vector2f moveVec2 = m_dir * m_speed;

	Vector2f MoveDottedVec2 = normalVec2 * normalVec2.Dot(moveVec2);
	Vector2f repulseVec2 = moveVec2 - MoveDottedVec2 * 2.0f;

	m_dir += normalVec2;
}

void GameObject::StoreTrace()
{
	GameObjectBase* traceGOB = new GameObject(*this);
	const TraceFrame& traceFrm = TraceFrame(traceGOB, m_myGame->GetGameTimeScale());
	m_trace.push_back(traceFrm);
}

void GameObject::RestoreTrace()
{
	if (m_trace.empty()) return;

	float fixedTs = m_myGame->GetFixedTimeScale();
	float nowTs = 0;
	int index = m_trace.size() - 1;

	while (index >= 0 && nowTs  < fixedTs)
	{
		nowTs += m_trace[index].timeScale;
		index--;
	}
	if (index != -1)
	{
		GameObjectBase* pStateGOB = m_trace[index].objectState;
		m_dir = pStateGOB->GetDirection();
		m_pos = pStateGOB->GetPosition();
		m_bActive = pStateGOB->GetActive();
		*m_pCollider = *pStateGOB->GetCollider();
	}

}

void GameObject::RemoveOldTrace()
{
	float slowTS = m_myGame->GetSlowFixedTimeScale();
	float rewindTime = m_myGame->GetRewindTime();
	int targetTraceSize = (int)(rewindTime / slowTS);
	while (m_trace.size() > targetTraceSize)
	{
		m_trace.front().Release();
		m_trace.pop_front();
	}
}

void GameObject::RemoveRecentTrace()
{
	if (m_trace.empty()) return;

	float fixedTs = m_myGame->GetFixedTimeScale();
	float nowTs = 0;
	int index = m_trace.size() - 1;

	while (index >= 0 && nowTs < fixedTs)
	{
		nowTs += m_trace[index].timeScale;
		m_trace.back().Release();
		m_trace.pop_back();
		index--;
	}
}

void GameObject::SetColliderCircle(float radius)
{
	using namespace learning;
	if (m_pCollider)
	{
		delete m_pCollider;
		m_pCollider = nullptr;
	}
	m_pCollider = new ColliderCircle(m_pos, radius);
	assert(m_pCollider != nullptr && "Failed to create ColliderCircle!");

}


void GameObject::SetColliderBox(float width, float height)
{
	using Vector2f = learning::Vector2f;

	if (m_pCollider)
	{
		delete m_pCollider;
		m_pCollider = nullptr;
	}
	Vector2f halfSize = Vector2f(width / 2.0f, height / 2.0f);
	m_pCollider = new ColliderBox(m_pos, halfSize);

	assert(m_pCollider != nullptr && "Failed to create ColliderBox!");
}




void GameObject::DrawCollider(HDC hdc)
{
	using learning::ColliderType;
	using learning::Collider;

	HPEN hPen = CreatePen(PS_SOLID, 2, m_colliderSolidColor);
	HBRUSH hBrush = CreateSolidBrush(m_colliderSolidColor);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	switch (m_pCollider->GetColliderType())
	{
		case ColliderType::CIRCLE:
		{
			ColliderCircle* pCirCol = dynamic_cast<ColliderCircle*>(m_pCollider);
			if (pCirCol == nullptr) break;
			Ellipse(hdc, pCirCol->m_center.x - pCirCol->m_radius,
				pCirCol->m_center.y - pCirCol->m_radius,
				pCirCol->m_center.x + pCirCol->m_radius,
				pCirCol->m_center.y + pCirCol->m_radius);
		}break;
		
		case ColliderType::BOX:
		{
			ColliderBox* pBoxCol = dynamic_cast<ColliderBox*>(m_pCollider);
			if (pBoxCol == nullptr) break;
			Rectangle(hdc, pBoxCol->m_center.x - pBoxCol->m_halfSize.x,
				pBoxCol->m_center.y - pBoxCol->m_halfSize.y,
				pBoxCol->m_center.x + pBoxCol->m_halfSize.x,
				pBoxCol->m_center.y + pBoxCol->m_halfSize.y);
		}break;
	}


	// 이전 객체 복원 및 펜 삭제
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void GameObject::Move(float deltaTime)
{
	GameObjectBase::Move(deltaTime);
}

GameObject::GameObject(const GameObject& other)
	:GameObjectBase(other)
{
}

//GameObject::GameObject(GameObject*)
//	:GameObjectBase()
//{
//}


