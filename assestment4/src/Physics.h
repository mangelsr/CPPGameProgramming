#include <memory>

#include "Entity.h"
#include "Vec2.h"

typedef std::pair<bool, Vec2> Intersect;

class Physics
{
public:
    static Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    static Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    static bool IsInside(const Vec2 &pos, std::shared_ptr<Entity> e);

    static Intersect LineIntersect(const Vec2 &a, const Vec2 &b, const Vec2 &c, const Vec2 &d);

    static bool EntityIntersect(const Vec2 &a, const Vec2 &b, std::shared_ptr<Entity> e);
};
