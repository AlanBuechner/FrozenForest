#ifndef COLLISION_H
#define COLLISION_H
#include <cstdint>

namespace Collision
{
    struct CollisionData
    {
        bool walkable;
        void (*interactCallback)(void) = nullptr;
    };

    void Init();

    CollisionData& GetCollisionData(uint32_t id);
}


#endif