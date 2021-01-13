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
        vector<Actor*> passActors;
        Material* mat;

        setRotation(cameraAngle);

        //opaque pass
        auto end = std::end(actors);
        for (auto act = std::begin(actors); act != end; ++act)
        {
            if (act->getMaterial()->transparency == 0.0f)
            {
#ifndef MULTITHREADING
                renderActor(&*act);
#endif // !MULTITHREADING
#ifdef MULTITHREADING
                pool.doJob(std::bind(Renderer::renderActor, &*act));
#endif // MULTITHREADING
                
            }
            else
            {
                passActors.push_back(&(*act));
            }
        }
#ifdef MULTITHREADING
        while (!pool.isFinished()) { /*wait for all threads to finish*/ }
#endif // MULTITHREADING

        //transparent pass
        auto end2 = std::end(passActors);
        for (auto act = std::begin(passActors); act != end2; ++act)
        {
#ifndef MULTITHREADING
            renderActor(*act);
#endif // !MULTITHREADING
#ifdef MULTITHREADING
            pool.doJob(std::bind(Renderer::renderActor, *act));
#endif // MULTITHREADING
        }
#ifdef MULTITHREADING
        while (!pool.isFinished()) { /*wait for all threads to finish*/ }
#endif // MULTITHREADING
    }

    static void renderActor(Actor* actor)
    {
        Material* const mat = actor->getMaterial();
        vector<pTriangle>* const tria = actor->getTriangles();
        Vector3* pos = actor->getPosition();
        TranslateMesh(actor->getVertices(), pos);
        Triangle* tmp;

        for (pTriangle tri : *tria)
        {
            tmp = pTriangle2Triangle(&tri, actor->getVertices(), mat->color);
            tdTriangle(*tmp, mat->transparency, &(mat->texture));
        }
    }

private:
    static float cosCamX, sinCamX;
    static float cosCamY, sinCamY;
    static float cosCamZ, sinCamZ;

protected:
    
    static Screen screen;

    static Triangle* pTriangle2Triangle(pTriangle* ptriangle, vector<Vertex>* vertices, Color color)
    {
        Triangle tri;
        tri.v1 = (*vertices)[ptriangle->v1].transformed;
        tri.v2 = (*vertices)[ptriangle->v2].transformed;
        tri.v3 = (*vertices)[ptriangle->v3].transformed;
        tri.vertexColor1 = color;
        tri.vertexColor2 = color;
        tri.vertexColor3 = color;
        tri.uv1 = ptriangle->uv1;
        tri.uv2 = ptriangle->uv2;
        tri.uv3 = ptriangle->uv3;
        return &tri;
    }

    static void setRotation(Vector3 angle)
    {
        cosCamX = cos(angle.x);
        cosCamY = cos(angle.y);
        cosCamZ = cos(angle.z);
        sinCamX = sin(angle.x);
        sinCamY = sin(angle.y);
        sinCamZ = sin(angle.z);
    }

    /*static Vector2* rotate2(float x, float y, float angle)
    {
        return &Vector2(x * cos(angle) + y * sin(angle), y * cos(angle) - x * sin(angle));
    }*/

    static Vector3* rotate(float x, float y, float z)
    {
        Vector3 coord(0, 0, 0);
        Vector2 rotated = Vector2(x * cosCamZ + y * sinCamZ, y * cosCamZ - x * sinCamZ); //rotate2(x, y, cameraAngle.z);
        coord.x = rotated.x;
        coord.y = rotated.y;
        rotated.UpdateV(coord.x * cosCamY + z * sinCamY, z * cosCamY - coord.x * sinCamY);// = rotate2(coord.x, z, cameraAngle.y);
        coord.x = rotated.x;
        coord.z = rotated.y;
        rotated.UpdateV(coord.z * cosCamX + coord.y * sinCamX, coord.y * cosCamX - coord.z * sinCamX);// rotate2(coord.z, coord.y, cameraAngle.x);
        coord.z = rotated.x;
        coord.y = rotated.y;
        return &coord;
    }

    static Vector3 TdToScreen(float x, float y, float z)
    {

        Vector3 rotated = *rotate(x, y, z);
        float multiplier = 0.5f * width / (((float)rotated.z + (float)dist) * fov);//0.5f * width / (((float)rotated.z + (float)dist) * fov);
        rotated.x *= multiplier;
        rotated.y *= multiplier;//(rotated.y / (rotated.z + dist));
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


    static void tdTriangle(Triangle triangle, float transparency, Mat* texture)
    {
        float factor = 255.0f / farMax;

        triangle.v1.z = (triangle.v1.z + dist) * factor;
        triangle.v2.z = (triangle.v2.z + dist) * factor;
        triangle.v3.z = (triangle.v3.z + dist) * factor;

        Screen::DrawTriangle(triangle.v1, triangle.v2, triangle.v3, (triangle.vertexColor1), (triangle.vertexColor2), (triangle.vertexColor3), transparency, texture, triangle.uv1, triangle.uv2, triangle.uv3);


        //debug
        //putText(image, to_string(p3v.z), *Screen::PosToScreenCenter(&Point(p3v.x, p3v.y)), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
    }

};

float Renderer::cosCamX = 0, Renderer::sinCamX = 0;
float Renderer::cosCamY = 0, Renderer::sinCamY = 0;
float Renderer::cosCamZ = 0, Renderer::sinCamZ = 0;
//Screen Renderer::screen;