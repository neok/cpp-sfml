#include "Physics.h"


vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) {
        return vec2(0, 0);
    }
    const vec2& posA = a->getComponent<CTransform>().pos;
    const vec2& posB = a->getComponent<CTransform>().pos;
    const vec2& aHalf = a->getComponent<CBoundingBox>().halfSize;
    const vec2& bHalf = b->getComponent<CBoundingBox>().halfSize;

    auto delta = vec2(std::abs(posA.x - posB.x), std::abs(posA.y - posB.y));

    const auto ox = aHalf.x + bHalf.x - delta.x;
    const auto oy = aHalf.y + bHalf.y - delta.y;

    return {ox, oy};
}

vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    // TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
    //       previous overlap uses the entity's previous position
    return vec2(0, 0);
}
