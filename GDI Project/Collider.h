#pragma once
#include"Utillity.h"


namespace learning
{
	enum class ColliderType
	{
		CIRCLE,
		BOX,
		NONE
	};

	class Collider
	{
	public:
		Collider() : m_colType(ColliderType::NONE) {}
		Collider(const Collider& other);


		virtual bool Intersect(Collider* lhs) const = 0;
		ColliderType GetColliderType() const { return m_colType; }
		void SetIsCollide(bool isCollide);
		bool IsColliding() const;
		virtual Collider* Clone() const = 0;
	
	public:
		Vector2f m_center;

	protected:
		ColliderType m_colType;
		bool m_isColliding = false;
	};

	class ColliderCircle : public Collider
	{
	public:
		ColliderCircle(Vector2f center, double radius)
			: m_radius(radius) {
			m_center = center;
			m_colType = ColliderType::CIRCLE;
		}
		ColliderCircle(const ColliderCircle& other)
			:ColliderCircle(other.m_center,other.m_radius)
		{

		}
		virtual Collider* Clone() const override
		{
			return new ColliderCircle(*this);
		}

		virtual bool Intersect(Collider* lhs) const override;
		double m_radius;
	};

	class ColliderBox : public Collider
	{
	public:
		ColliderBox(Vector2f center, Vector2f halfSize) 
			:m_halfSize(halfSize) 
		{
			m_center = center;
			m_colType = ColliderType::BOX;
		}

		ColliderBox(const ColliderBox& other)
			:ColliderBox(other.m_center,other.m_halfSize)
		{

		}
		virtual Collider* Clone() const override
		{
			return new ColliderBox(*this);
		}

		virtual bool Intersect(Collider* lhs) const override;
		Vector2f m_halfSize;

	};

	//bool Intersect(ColliderCircle const& lhs, ColliderCircle const& rhs);
	//bool Intersect(ColliderBox const& lhs, ColliderBox const& rhs);
}