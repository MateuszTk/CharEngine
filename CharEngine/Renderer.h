#pragma once

#include "Helper.h"
#include "Screen.h"
#include <vector>
#include "Actor.h"

//using namespace helper;
using namespace cv;


vector<Actor> actors;
vector<Texture> textures;
vector<Tile> tiles;

class Renderer
{

public:

    virtual void Update() = 0;
    virtual void Start() = 0;

    static void render()
    {
        vector<Actor*> passActors;
        clearTiles();
        setRotation(cameraAngle);

        //opaque pass
        auto end = std::end(actors);
        for (auto act = std::begin(actors); act != end; ++act)
        {
            if (act->getMaterial()->transparency == 0.0f)
            {
                pool.doJob(std::bind(submitActor, &*act));
            }
            else
            {
                //passActors.push_back(&(*act));
            }
        }

        while (!pool.isFinished()) { /*wait for all threads to finish*/ }

        /*auto end2 = std::end(passActors);
        for (auto act = std::begin(passActors); act != end2; ++act)
        {
            pool.doJob(std::bind(submitActor, *act));
        }*/

        //int q = 0;
        auto end0 = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != end0; ++tile)
        {
            //if(q%2 == 0)
            pool.doJob(std::bind(renderTile, &*tile));
           // q++;
        }

        while (!pool.isFinished()) { /*wait for all threads to finish*/ }

        /*//transparent pass
        end0 = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != end0; ++tile)
        {
            //if(q%2 == 0)
            pool.doJob(std::bind(renderTile, &*tile));
           // q++;
        }*/

    }

    static void submitActor(Actor* actor)
    {
        vector<pTriangle>* const tria = actor->getTriangles();
        TranslateMesh(actor->getVertices(), actor->getPosition());
        //Triangle* tmp;

        auto end = std::end(*tria);
        for (auto tri = std::begin(*tria); tri != end; ++tri)
        {
            pTriangle2Triangle(&*tri, actor->getVertices(), actor->getMaterial(), tri->triangleData);
            //tdTriangle(*tmp, mat->transparency, &(textures[mat->textureId].textureData));
        }
    }

    static void initializeTiles()
    {
        Tile tile;
        for (int x = 0; x < numberOfTilesX; x++)
        {
            for (int y = 0; y < numberOfTilesY; y++)
            {
                tile.pmin = Point(x * tileWidth, y * tileHeight);
                tile.pmax = Point(x * tileWidth + tileWidth, y * tileHeight + tileHeight);
                tiles.push_back(tile);
                tiles[x].assignedTriangles.reserve(100000);
                for (int i = 0; i < 100000; i++)
                {
                    tiles[x].assignedTriangles.push_back(0);
                }
            }
        }
    }

private:
    static float cosCamX, sinCamX;
    static float cosCamY, sinCamY;
    static float cosCamZ, sinCamZ;

protected:
    
    static Screen screen;

    static std::mutex lock;
    //static vector<Triangle> triangles;

    static void renderTile(Tile* tile)
    {
        //auto end0 = std::end(tile->assignedTriangles);
        for (int i = tile->aT_len - 1; i >= 0; i--)//for (auto tilei = std::begin(tile->assignedTriangles); tilei != end0; ++tilei)
        {
            tdTriangle(*(tile->assignedTriangles[i]), tile);
        }
    }

    static void clearTiles()
    {
        auto endt = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != endt; ++tile)
        {
            tile->aT_len = 0;//tile->assignedTriangles.clear();//
        }
    }


    static void pTriangle2Triangle(pTriangle* ptriangle, vector<Vertex>* vertices, Material* mat, Triangle& tri)
    {
        //Triangle tri;
        tri.v0 = (*vertices)[ptriangle->v1].transformed;
        tri.v1 = (*vertices)[ptriangle->v2].transformed;
        tri.v2 = (*vertices)[ptriangle->v3].transformed;

        float factor = 255.0f / farMax;
        tri.v0.z = (tri.v0.z + dist) * factor;
        tri.v1.z = (tri.v1.z + dist) * factor;
        tri.v2.z = (tri.v2.z + dist) * factor;

        if (tri.v0.z <= clipNear || tri.v1.z <= clipNear || tri.v2.z <= clipNear || tri.v0.z > farMax || tri.v1.z > farMax || tri.v2.z > farMax)
            return;

        Screen::PosToScreenCenter(&tri.v0);
        Screen::PosToScreenCenter(&tri.v1);
        Screen::PosToScreenCenter(&tri.v2);

        tri.materialp = mat;
        tri.uv0 = ptriangle->uv1;
        tri.uv1 = ptriangle->uv2;
        tri.uv2 = ptriangle->uv3;

        //bounding box
        tri.bbmax = Point(0, 0);
        tri.bbmin = Point(width - 1, height - 1);
        Vector3* points[3] = { &tri.v0, &tri.v1, &tri.v2 };
        for (int i = 0; i < 3; ++i)
        {
            if (points[i]->x < tri.bbmin.x)
                tri.bbmin.x = points[i]->x;
            if (points[i]->y < tri.bbmin.y)
                tri.bbmin.y = points[i]->y;
            if (points[i]->x > tri.bbmax.x)
                tri.bbmax.x = points[i]->x;
            if (points[i]->y > tri.bbmax.y)
                tri.bbmax.y = points[i]->y;
        }
        if (tri.bbmax.x > width - 1) tri.bbmax.x = width - 1;
        if (tri.bbmax.y > height - 1) tri.bbmax.y = height - 1;
        if (tri.bbmin.x < 0) tri.bbmin.x = 0;
        if (tri.bbmin.y < 0) tri.bbmin.y = 0;
        //SetPixelColor(&bbmin, &Color(0, 255, 0));
        //SetPixelColor(&bbmax, &Color(255, 0, 0));

        //single triangle binning
        int t = tri.bbmax.x / tileWidth;
        t = (t > numberOfTilesX - 1) ? numberOfTilesX - 1: t;
        for (int x = tri.bbmin.x / tileWidth; x <= t; x++)
        {
            //for (int y = floorf(tri.bbmin.y / tileHeight); y < numberOfTilesY; y++)//tiles[x + y * numberOfTilesX]
            lock.lock();
            tiles[x].assignedTriangles[tiles[x].aT_len] = &tri;
            tiles[x].aT_len++;
            lock.unlock();

        }
        
        //return tri;
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

    static Vector3* rotateWorld(float x, float y, float z)
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

        Vector3 rotated = *rotateWorld(x, y, z);
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


    static void tdTriangle(Triangle triangle, Tile* tile)
    {
        

        Screen::DrawTriangle(triangle, tile, &(textures[(triangle).materialp->textureId].textureData));
        //Screen::DrawTriangle(triangle.v0, triangle.v1, triangle.v2, (triangle.materialp->color), (triangle.materialp->color), (triangle.materialp->color), (triangle.materialp->transparency), &textures[triangle.materialp->textureId].textureData, triangle.uv0, triangle.uv1, triangle.uv2);

        //debug
        //putText(image, to_string(p3v.z), *Screen::PosToScreenCenter(&Point(p3v.x, p3v.y)), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
    }

};

float Renderer::cosCamX = 0, Renderer::sinCamX = 0;
float Renderer::cosCamY = 0, Renderer::sinCamY = 0;
float Renderer::cosCamZ = 0, Renderer::sinCamZ = 0;
std::mutex Renderer::lock;
//vector<Triangle> Renderer::triangles;
//Screen Renderer::screen;