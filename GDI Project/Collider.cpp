#include "Collider.h"

#ifdef _DEBUG
#include<iostream>
#endif 

learning::Collider::Collider(const Collider& other)
	:m_colType{ other.m_colType },
	m_isColliding(other.m_isColliding)
	//m_center(other.m_center)
{
	m_center = other.m_center;
}

bool learning::ColliderCircle::Intersect(Collider* rhs)const
{
	ColliderCircle* rCir = dynamic_cast<ColliderCircle*>(rhs);
	if (rCir == nullptr)
	{
		std::cout << "ColliderCircle 형변환 실패";
		return false;
	}
	return (rCir->m_center - m_center).LengthSquared() <= pow(rCir->m_radius + m_radius, 2);
}

void learning::Collider::SetIsCollide(bool isCollide)
{
	m_isColliding = isCollide;
}

bool learning::Collider::IsColliding() const
{
	return m_isColliding;
}


bool learning::ColliderBox::Intersect(Collider* rhs)const
{
	ColliderBox* rBox = dynamic_cast<ColliderBox*>(rhs);
	if (rBox == nullptr)
	{
		std::cout << "ColliderBox 형변환  실패";
		return false;
	}

	if (rBox->m_center.x - rBox->m_halfSize.x > m_center.x + m_halfSize.x) return false;
	if (rBox->m_center.x + rBox->m_halfSize.x < rBox->m_center.x - rBox->m_halfSize.x) return false;

	if (rBox->m_center.y - rBox->m_halfSize.y > m_center.y + m_halfSize.y) return false;
	if (rBox->m_center.y + rBox->m_halfSize.y < m_center.y - m_halfSize.y) return false;
	return true;
}