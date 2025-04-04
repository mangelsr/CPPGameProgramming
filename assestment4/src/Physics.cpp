#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // STUDENT TODO:
    // Implement this function
    return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // STUDENT TODO:
    // Implement this function
    return Vec2(0, 0);
}

bool Physics::IsInside(const Vec2 &pos, std::shared_ptr<Entity> e)
{
    // STUDENT TODO:
    // Implement this function
    return false;
}

Intersect Physics::LineIntersect(const Vec2 &a, const Vec2 &b, const Vec2 &c, const Vec2 &d)
{
    // STUDENT TODO:
    // Implement this function
    return {false, Vec2(0, 0)};
}

bool Physics::EntityIntersect(const Vec2 &a, const Vec2 &b, std::shared_ptr<Entity> e)
{
    // STUDENT TODO:
    // Implement this function
    return false;
}