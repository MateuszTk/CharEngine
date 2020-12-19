#pragma once

#include "Helper.h"
#include "Screen.h"
#include <vector>
#include "Actor.h"

//using namespace helper;
using namespace cv;


vector<Actor> actors;

class Renderer
{

public:
    virtual void Update() = 0;
    virtual void Start() = 0;

    static void render()
    {
        vector<Triangle> geom;
        vector<Actor*> passActors;
        Vector3* pos;
        Material* mat;
        Color c1 = Color(255, 0, 255);
        Color c2 = Color(255, 0, 255);
        Color c3 = Color(255, 0, 255);

        //opaque pass
        auto end = std::end(actors);
        for (auto act = std::begin(actors); act != end; ++act)
        {
            mat = act->getMaterial();
            if (mat->transparency == 0.0f)
            {
                geom = *(act->getGeometry());
                pos = act->getPosition();

                for (Triangle tri : geom)
                {
                    tri.v1.x += pos->x;
                    tri.v1.y += pos->y;
                    tri.v1.z += pos->z;

                    tri.v2.x += pos->x;
                    tri.v2.y += pos->y;
                    tri.v2.z += pos->z;

                    tri.v3.x += pos->x;
                    tri.v3.y += pos->y;
                    tri.v3.z += pos->z;

                    c1 = mat->color;
                    c2 = mat->color;
                    c3 = mat->color;

                    tri.vertexColor1 = c1;
                    tri.vertexColor2 = c2;
                    tri.vertexColor3 = c3;
                    tdTriangle(&tri, 0.0f);
                }
            }
            else
                passActors.push_back(&(*act));
        }

        //transparent pass
        auto end2 = std::end(passActors);
        for (auto act = std::begin(passActors); act != end2; ++act)
        {
            mat = (*act)->getMaterial();

            geom = *((*act)->getGeometry());
            pos = (*act)->getPosition();

            for (Triangle tri : geom)
            {
                tri.v1.x += pos->x;
                tri.v1.y += pos->y;
                tri.v1.z += pos->z;

                tri.v2.x += pos->x;
                tri.v2.y += pos->y;
                tri.v2.z += pos->z;

                tri.v3.x += pos->x;
                tri.v3.y += pos->y;
                tri.v3.z += pos->z;

                c1 = mat->color;
                c2 = mat->color;
                c3 = mat->color;

                tri.vertexColor1 = c1;
                tri.vertexColor2 = c2;
                tri.vertexColor3 = c3;
                tdTriangle(&tri, mat->transparency);
            }
        }
    }



protected:
    
    static Vector2* rotate2(float x, float y, float angle)
    {
        return &Vector2(x * cos(angle) + y * sin(angle), y * cos(angle) - x * sin(angle));
    }

    static Vector3* rotate(float x, float y, float z)
    {
        Vector3 coord(0, 0, 0);
        Vector2* rotated = rotate2(x, y, cameraAngle.z);
        coord.x = rotated->x;
        coord.y = rotated->y;
        rotated = rotate2(coord.x, z, cameraAngle.y);
        coord.x = rotated->x;
        coord.z = rotated->y;
        rotated = rotate2(coord.z, coord.y, cameraAngle.x);
        coord.z = rotated->x;
        coord.y = rotated->y;
        return &coord;
    }

    static Vector3* TdToScreen(float x, float y, float z)
    {

        Vector3* rotated = rotate(x, y, z);
        float multiplier = 0.5f * width / (((float)rotated->z + (float)dist) * fov);
        rotated->x *= multiplier;
        rotated->y *= multiplier;
        return rotated;
    }


    static void tdTriangle(Triangle* triangle, float transparency)
    {
        Vector3 p1v = *TdToScreen(triangle->v1.x + cameraPosition.x, triangle->v1.y + cameraPosition.y, triangle->v1.z + cameraPosition.z);
        Vector3 p2v = *TdToScreen(triangle->v2.x + cameraPosition.x, triangle->v2.y + cameraPosition.y, triangle->v2.z + cameraPosition.z);
        Vector3 p3v = *TdToScreen(triangle->v3.x + cameraPosition.x, triangle->v3.y + cameraPosition.y, triangle->v3.z + cameraPosition.z);

        float factor = 255.0f / farMax;

        p1v.z = (p1v.z + dist) * factor;
        p2v.z = (p2v.z + dist) * factor;
        p3v.z = (p3v.z + dist) * factor;

        Screen::DrawTriangle(p1v, p2v, p3v, (triangle->vertexColor1), (triangle->vertexColor2), (triangle->vertexColor3), transparency);

        //debug
        //putText(image, to_string(p3v.z), *Screen::PosToScreenCenter(&Point(p3v.x, p3v.y)), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
    }
};
