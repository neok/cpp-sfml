#include "Physics.h"


vec2 Physics::GetOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b) {
    if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) {
        return vec2(0, 0);
    }
    const vec2 posA = a->getComponent<CTransform>().pos;
    const vec2 posB = b->getComponent<CTransform>().pos;
    const vec2 aHalf = a->getComponent<CBoundingBox>().halfSize;
    const vec2 bHalf = b->getComponent<CBoundingBox>().halfSize;

    auto delta = vec2(std::abs(posA.x - posB.x), std::abs(posA.y - posB.y));

    const auto ox = aHalf.x + bHalf.x - delta.x;
    const auto oy = aHalf.y + bHalf.y - delta.y;

    return {ox, oy};
}

vec2 Physics::GetPreviousOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b) {
    if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) {
        return vec2(0, 0);
    }
    const vec2 posA = a->getComponent<CTransform>().prevPos;
    const vec2 posB = b->getComponent<CTransform>().prevPos;
    const vec2 aHalf = a->getComponent<CBoundingBox>().halfSize;
    const vec2 bHalf = b->getComponent<CBoundingBox>().halfSize;

    auto delta = vec2(std::abs(posA.x - posB.x), std::abs(posA.y - posB.y));

    const auto ox = aHalf.x + bHalf.x - delta.x;
    const auto oy = aHalf.y + bHalf.y - delta.y;

    return {ox, oy};
}
