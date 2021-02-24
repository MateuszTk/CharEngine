#pragma once

#include "Helper.h"
#include "Screen.h"
#include <vector>
#include "Actor.h"

#ifdef MULTITHREADING
#   define DO_ACTOR_JOB(job) pool.doJob(std::bind(submitActor, job));
#   define DO_TILE_JOB(job) pool.doJob(std::bind(renderTile, job));
#   define WAIT_FOR_JOBS() while (!pool.isFinished()) {}
#else
#   define DO_ACTOR_JOB(job) submitActor(job);
#   define DO_TILE_JOB(job) renderTile(job);
#   define WAIT_FOR_JOBS()
#endif

vector<Actor> actors;
vector<Texture> textures;
vector<Tile> tiles(numberOfTilesX);

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
#if defined(TRANSPARENCY)
            if (act->getMaterial()->transparency == 0.0f)
            {
                DO_ACTOR_JOB( &*act );
            }
            else
            {
                passActors.push_back(&(*act));
            }
#else
            DO_ACTOR_JOB( &*act );
#endif

            
        }

        WAIT_FOR_JOBS();

        //int q = 0;
        auto end0 = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != end0; ++tile)
        {
            //if(q%2 == 0)
            DO_TILE_JOB( &*tile );
           // q++;
        }

        WAIT_FOR_JOBS();

#if defined(TRANSPARENCY)
        //transparent pass
        auto end2 = std::end(passActors);
        for (auto act = std::begin(passActors); act != end2; ++act)
        {
            DO_ACTOR_JOB( &*act );
        }

        WAIT_FOR_JOBS();

        end0 = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != end0; ++tile)
        {
            DO_TILE_JOB( &*tile );
        }

        WAIT_FOR_JOBS();
#endif

    }

    static void submitActor(Actor* actor)
    {
        vector<pTriangle>* const tria = actor->getTriangles();
        auto type = actor->getActorType();
        TranslateMesh(actor->getVertices(), actor->getPosition(), type);

        auto end = std::end(*tria);
        for (auto tri = std::begin(*tria); tri != end; ++tri)
        {
            pTriangle2Triangle(&*tri, actor->getVertices(), actor->getMaterial(), tri->triangleData, type);
        }
    }

    static void initializeTiles()
    {
        for (int x = 0; x < numberOfTilesX; x++)
        {
            for (int y = 0; y < numberOfTilesY; y++)
            {
                tiles[x].pmin = Point(x * tileWidth, y * tileHeight);
                tiles[x].pmax = Point(x * tileWidth + tileWidth, y * tileHeight + tileHeight);

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

    static void renderTile(Tile* tile)
    {
        const int len = tile->aT_len;
        for (int i = 0; i < len; i++)
        {
            tdTriangle(*(tile->assignedTriangles[i]), tile);
        }
    }

    static void clearTiles()
    {
        auto endt = std::end(tiles);
        for (auto tile = std::begin(tiles); tile != endt; ++tile)
        {
            tile->aT_len = 0;
        }
    }


    static void pTriangle2Triangle(pTriangle* ptriangle, vector<Vertex>* vertices, Material* mat, Triangle& tri, ActorType* type)
    {
        //Triangle tri;
        tri.v0 = (*vertices)[ptriangle->v1].transformed;
        tri.v1 = (*vertices)[ptriangle->v2].transformed;
        tri.v2 = (*vertices)[ptriangle->v3].transformed;

        float factor = 255.0f / farMax;
        if (*type != ActorType::Skybox)
        {
            tri.v0.z = (tri.v0.z + dist) * factor;
            tri.v1.z = (tri.v1.z + dist) * factor;
            tri.v2.z = (tri.v2.z + dist) * factor;
        }
        else
        {
            tri.v0.z *= factor;
            tri.v1.z *= factor;
            tri.v2.z *= factor;
        }

        //clipping plane intersection
        if (tri.v0.z <= clipNear || tri.v1.z <= clipNear || tri.v2.z <= clipNear || tri.v0.z > farMax || tri.v1.z > farMax || tri.v2.z > farMax)
            return;


        //===========================
        Screen::PosToScreenCenter(&tri.v0);
        Screen::PosToScreenCenter(&tri.v1);
        Screen::PosToScreenCenter(&tri.v2);

        //face normal check
        Vector3 a(tri.v1.x - tri.v0.x, tri.v1.y - tri.v0.y, tri.v1.z - tri.v0.z); //v1 - v0
        Vector3 b(tri.v2.x - tri.v0.x, tri.v2.y - tri.v0.y, tri.v2.z - tri.v0.z); //v2 - v0
        tri.normal.z = a.x * b.y - a.y * b.x;
        if (tri.normal.z >= 0)
            return;

        tri.normal.x = a.y * b.z - a.z * b.y;
        tri.normal.y = a.z * b.x - a.x * b.z;
        tri.normal.Normalize();

        tri.materialp = mat;
        tri.uv0 = ptriangle->uv1;
        tri.uv1 = ptriangle->uv2;
        tri.uv2 = ptriangle->uv3;

        tri.type = *type;

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
            tiles[x].TS_addTriangle(&tri);
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

    static void rotateWorld(float x, float y, float z, Vector3* vec)
    {
        Vector2 rotated = Vector2(x * cosCamZ + y * sinCamZ, y * cosCamZ - x * sinCamZ);
        vec->x = rotated.x;
        vec->y = rotated.y;

        rotated.UpdateV(vec->x * cosCamY + z * sinCamY, z * cosCamY - vec->x * sinCamY);
        vec->x = rotated.x;
        vec->z = rotated.y;

        rotated.UpdateV(vec->z * cosCamX + vec->y * sinCamX, vec->y * cosCamX - vec->z * sinCamX);
        vec->z = rotated.x;
        vec->y = rotated.y;
    }

    static Vector3 TdToScreen(float x, float y, float z, float cdist)
    {
        Vector3 rotated(0, 0, 0);
        rotateWorld(x, y, z, &rotated);

        float multiplier = -(0.5f * width / ((rotated.z + cdist) * fov));//-fabsf
        rotated.x *= multiplier;
        rotated.y *= multiplier;

        return std::move(rotated);
    }

    static void TranslateMesh(std::vector<Vertex>* vertices, Vector3* position, ActorType* atype)
    {
        auto end = std::end(*vertices);
        if (*atype == ActorType::Common)
        {
            for (auto vertex = std::begin(*vertices); vertex != end; ++vertex)
            {
                vertex->transformed = TdToScreen(vertex->raw.x + cameraPosition.x + position->x, vertex->raw.y + cameraPosition.y + position->y, vertex->raw.z + cameraPosition.z + position->z, dist);
            }
        }
        else
        {
            for (auto vertex = std::begin(*vertices); vertex != end; ++vertex)
            {
                vertex->transformed = TdToScreen(vertex->raw.x, vertex->raw.y, vertex->raw.z, 0.0f);
            }
        }
    }


    static void tdTriangle(Triangle triangle, Tile* tile)
    {
        Screen::DrawTriangle(triangle, tile, &(textures[(triangle).materialp->textureId].textureData));

        //debug
        //putText(image, to_string(p3v.z), *Screen::PosToScreenCenter(&Point(p3v.x, p3v.y)), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
    }

};

float Renderer::cosCamX = 0, Renderer::sinCamX = 0;
float Renderer::cosCamY = 0, Renderer::sinCamY = 0;
float Renderer::cosCamZ = 0, Renderer::sinCamZ = 0;
