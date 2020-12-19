#pragma once

#include <vector>
#include "Helper.h"

class Actor
{
private:
    std::vector<Triangle> geometry;
    Vector3 position = Vector3(0, 0, 0);
    std::string name;
    Material material;

public:
    std::vector<Triangle>* getGeometry()
    {
        return &geometry;
    }

    Vector3* getPosition()
    {
        return &position;
    }

    std::string* getName()
    {
        return &name;
    }

    Material* getMaterial()
    {
        return &material;
    }

    static vector<Actor*> Find(std::vector<std::string>* names, std::vector<Actor>* actors)
    {
        vector<Actor*> found;
        auto end = std::end(*actors);
        for (auto act = std::begin(*actors); act != end; ++act)
        {
            for (auto name : *names)
            {
                if (*(act->getName()) == name)
                {
                    found.push_back(&(*act));
                }
            }
        }
        return found;
    }

    static void SetGroupPosition(vector<Actor*>* group, Vector3* newPosition)
    {
        auto end = std::end(*group);
        for (auto act = std::begin(*group); act != end; ++act)
        {
            *((**act).getPosition()) = *newPosition;
        }
    }

    static Actor createCube(Vector3 pos, Material* mat)
    {
        Actor actor;
        *(actor.getPosition()) = pos;
        Triangle tri;
        actor.material = *mat;

        tri.v1.UpdateV(-1, -1, 1);
        tri.v2.UpdateV(-1, 1, 1);
        tri.v3.UpdateV(1, -1, 1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(1, 1, 1);
        tri.v2.UpdateV(1, -1, 1);
        tri.v3.UpdateV(-1, 1, 1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(-1, -1, -1);
        tri.v2.UpdateV(1, -1, -1);
        tri.v3.UpdateV(-1, 1, -1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(1, 1, -1);
        tri.v2.UpdateV(-1, 1, -1);
        tri.v3.UpdateV(1, -1, -1);
        actor.getGeometry()->push_back(tri);

        //-------------------------------------------------//

        tri.v1.UpdateV(1, -1, -1);
        tri.v2.UpdateV(1, -1, 1);
        tri.v3.UpdateV(1, 1, -1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(1, 1, 1);
        tri.v2.UpdateV(1, 1, -1);
        tri.v3.UpdateV(1, -1, 1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(-1, -1, -1);
        tri.v2.UpdateV(-1, 1, -1);
        tri.v3.UpdateV(-1, -1, 1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(-1, 1, 1);
        tri.v2.UpdateV(-1, -1, 1);
        tri.v3.UpdateV(-1, 1, -1);
        actor.getGeometry()->push_back(tri);

        //-------------------------------------------------//

        tri.v1.UpdateV(-1, 1, -1);
        tri.v2.UpdateV(1, 1, -1);
        tri.v3.UpdateV(-1, 1, 1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(1, 1, 1);
        tri.v2.UpdateV(-1, 1, 1);
        tri.v3.UpdateV(1, 1, -1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(-1, -1, -1);
        tri.v2.UpdateV(-1, -1, 1);
        tri.v3.UpdateV(1, -1, -1);
        actor.getGeometry()->push_back(tri);

        tri.v1.UpdateV(1, -1, 1);
        tri.v2.UpdateV(1, -1, -1);
        tri.v3.UpdateV(-1, -1, 1);
        actor.getGeometry()->push_back(tri);

        return actor;
    }
};