#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: return the overlap rectangle size of the bounding boxes of entity a and b
    Vec2 aPos = a->getComponent<CTransform>().pos;
    Vec2 aHalfBBox = a->getComponent<CBoundingBox>().halfSize;

    Vec2 bPos = b->getComponent<CTransform>().pos;
    Vec2 bHalfBBox = b->getComponent<CBoundingBox>().halfSize;

    Vec2 delta(std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y));

    return aHalfBBox + bHalfBBox - delta;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
    // previous overlap uses the entity's previous position
    Vec2 aPos = a->getComponent<CTransform>().prevPos;
    Vec2 aHalfBBox = a->getComponent<CBoundingBox>().halfSize;

    Vec2 bPos = b->getComponent<CTransform>().prevPos;
    Vec2 bHalfBBox = b->getComponent<CBoundingBox>().halfSize;

    Vec2 delta(std::abs(aPos.x - bPos.x), std::abs(aPos.y - bPos.y));

    return aHalfBBox + bHalfBBox - delta;
}