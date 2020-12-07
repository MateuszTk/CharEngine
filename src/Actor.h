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

    static Actor createCube(Vector3 pos, Color* col1, Color* col2, Color* col3)
    {
        Actor actor;
        *(actor.getPosition()) = pos;
        Triangle tri;

        tri.v1.UpdateV(-1, -1, 1);
        tri.v2.UpdateV(-1, 1, 1);
        tri.v3.UpdateV(1, -1, 1);
        tri.vertexColor1 = *col1;
        tri.vertexColor2 = *col2;
        tri.vertexColor3 = *col3;
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