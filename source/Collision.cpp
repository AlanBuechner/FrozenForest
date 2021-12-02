#include "Collision.h"
#include "InteractionCallbacks.h"

namespace Collision
{
    constexpr uint32_t collisionTypes = 4;
    CollisionData data[collisionTypes];

    void Init()
    {
        data[0].walkable = true;
        data[1].walkable = false;
        data[2].walkable = false;
        data[2].interactCallback = interaction1;
    }

    CollisionData& GetCollisionData(uint32_t id)
    {
        if(id >= collisionTypes)
            return data[0];

        return data[id];
    }
}