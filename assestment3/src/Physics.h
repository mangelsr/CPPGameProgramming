#include <memory>

#include "Entity.h"
#include "Vec2.h"

class Physics
{
public:
    // Calculate the overlap rectangle size between two entities' bounding boxes
    static Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    // Calculate the previous overlap rectangle size between two entities' bounding boxes
    // using their previous positions
    static Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};
