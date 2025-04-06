#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 aPos = a->getComponent<CTransform>().pos;
    Vec2 aHalfBBox = a->getComponent<CBoundingBox>().halfSize;

    Vec2 bPos = b->getComponent<CTransform>().pos;
    Vec2 bHalfBBox = b->getComponent<CBoundingBox>().halfSize;

    Vec2 delta(std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y));

    return aHalfBBox + bHalfBBox - delta;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 aPrevPos = a->getComponent<CTransform>().prevPos;
    Vec2 aHalfBBox = a->getComponent<CBoundingBox>().halfSize;

    Vec2 bPrevPos = b->getComponent<CTransform>().prevPos;
    Vec2 bHalfBBox = b->getComponent<CBoundingBox>().halfSize;

    Vec2 delta(std::abs(aPrevPos.x - bPrevPos.x), std::abs(aPrevPos.y - bPrevPos.y));

    return aHalfBBox + bHalfBBox - delta;
}

bool Physics::IsInside(const Vec2 &pos, std::shared_ptr<Entity> e)
{
    Vec2 ePos = e->getComponent<CTransform>().pos;
    Vec2 halfSize = e->getComponent<CAnimation>().animation.getSize() / 2;

    float dx = std::abs(pos.x - ePos.x);
    float dy = std::abs(pos.y - ePos.y);

    return (dx <= halfSize.x) && (dy <= halfSize.y);
}

Intersect Physics::LineIntersect(const Vec2 &a, const Vec2 &b, const Vec2 &c, const Vec2 &d)
{
    Vec2 r(b - a);
    Vec2 s(d - c);
    float rxs = r.cross(s);
    Vec2 cma(c - a);
    float t = cma.cross(s) / rxs;
    float u = cma.cross(r) / rxs;
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        return {true, Vec2(a.x + t * r.x, a.y + t * r.y)};
    }
    else
    {
        return {false, Vec2(0, 0)};
    }
}

bool Physics::EntityIntersect(const Vec2 &a, const Vec2 &b, std::shared_ptr<Entity> e)
{
    Vec2 ePos = e->getComponent<CTransform>().pos;
    Vec2 halfSize = e->getComponent<CAnimation>().animation.getSize() / 2;

    Vec2 e1(ePos.x - halfSize.x, ePos.y - halfSize.y);
    Vec2 e2(ePos.x + halfSize.x, ePos.y - halfSize.y);
    Vec2 e3(ePos.x + halfSize.x, ePos.y + halfSize.y);
    Vec2 e4(ePos.x - halfSize.x, ePos.y + halfSize.y);

    if (LineIntersect(a, b, e1, e2).first ||
        LineIntersect(a, b, e2, e3).first ||
        LineIntersect(a, b, e3, e4).first ||
        LineIntersect(a, b, e4, e1).first)
    {
        return true;
    }

    return false;
}