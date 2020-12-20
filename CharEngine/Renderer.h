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
        vector<pTriangle> tria;
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
                tria = *(act->getTriangles());
                pos = act->getPosition();
                TranslateMesh(act->getVertices(), pos);

                for (pTriangle tri : tria)
                {
                    tdTriangle(pTriangle2Triangle(&tri, act->getVertices(), mat->color), 0.0f);
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

            tria = *((*act)->getTriangles());
            pos = (*act)->getPosition();
            TranslateMesh((*act)->getVertices(), pos);

            for (pTriangle tri : tria)
            {            
                tdTriangle(pTriangle2Triangle(&tri, (*act)->getVertices(), mat->color), mat->transparency);
            }
        }
    }



protected:
    
    static Triangle pTriangle2Triangle(pTriangle* ptriangle, vector<Vertex>* vertices, Color color)
    {
        Triangle tri;
        tri.v1 = (*vertices)[ptriangle->v1].transformed;
        tri.v2 = (*vertices)[ptriangle->v2].transformed;
        tri.v3 = (*vertices)[ptriangle->v3].transformed;
        tri.vertexColor1 = color;
        tri.vertexColor2 = color;
        tri.vertexColor3 = color;
        return tri;
    }

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

    static Vector3 TdToScreen(float x, float y, float z)
    {

        Vector3 rotated = *rotate(x, y, z);
        float multiplier = 0.5f * width / (((float)rotated.z + (float)dist) * fov);
        rotated.x *= multiplier;
        rotated.y *= multiplier;
        return rotated;
    }

    static void TranslateMesh(std::vector<Vertex>* vertices, Vector3* position)
    {
        auto end = std::end(*vertices);
        for (auto vertex = std::begin(*vertices); vertex != end; ++vertex)
        {
            vertex->transformed = TdToScreen(vertex->raw.x + cameraPosition.x + position->x, vertex->raw.y + cameraPosition.y + position->y, vertex->raw.z + cameraPosition.z + position->z);
        }
    }


    static void tdTriangle(Triangle triangle, float transparency)
    {
        float factor = 255.0f / farMax;

        triangle.v1.z = (triangle.v1.z + dist) * factor;
        triangle.v2.z = (triangle.v2.z + dist) * factor;
        triangle.v3.z = (triangle.v3.z + dist) * factor;

        Screen::DrawTriangle(triangle.v1, triangle.v2, triangle.v3, (triangle.vertexColor1), (triangle.vertexColor2), (triangle.vertexColor3), transparency);

        //debug
        //putText(image, to_string(p3v.z), *Screen::PosToScreenCenter(&Point(p3v.x, p3v.y)), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
    }
};
