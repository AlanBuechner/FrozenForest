#include "Collision.h"
#include "InteractionCallbacks.h"

namespace Collision
{
    constexpr uint32_t collisionTypes = 9;
    CollisionData data[collisionTypes];

    void Init()
    {
        data[0].walkable = true;
        data[1].walkable = false;
        data[2].walkable = false;
        data[3].walkable = false;
        data[4].walkable = false;
        data[5].walkable = false;
        data[6].walkable = false;
        data[7].walkable = false;
        data[8].walkable = false;
       


        data[2].interactCallback = interaction1;
        data[3].interactCallback = interaction2;
        data[4].interactCallback = interaction3;
        data[5].interactCallback = interaction4;
        data[6].interactCallback = interaction5;
        data[7].interactCallback = interaction6;
        data[8].interactCallback = interaction7;




    }

    CollisionData& GetCollisionData(uint32_t id)
    {
        if(id >= collisionTypes)
            return data[0];

        return data[id];
    }
}