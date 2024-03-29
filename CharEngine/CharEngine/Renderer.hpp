#pragma once

#include "Helper.hpp"
#include "Screen.hpp"
#include "Actor.hpp"

#ifdef MULTITHREADING
#   define DO_ACTOR_JOB(job) Global::pool.doJob(std::bind(submitActor, job));
#   define DO_TILE_JOB(job) Global::pool.doJob(std::bind(renderTile, job));
#   define WAIT_FOR_JOBS() waitForJobFun()
#else
#   define DO_ACTOR_JOB(job) submitActor(job);
#   define DO_TILE_JOB(job) renderTile(job);
#   define WAIT_FOR_JOBS()
#endif
namespace CharEngine {
    namespace Global {
        vector<Actor> actors;
        vector<Texture> textures;
        vector<Tile> tiles(Config::numberOfTilesX);
    }

    class Renderer : private Screen{

    public:

        static void render(CharEngine::Camera& camera) {
            currentCamera = &camera;

            vector<Actor*> passActors;
            emptyTiles();
            setRotation(currentCamera->angle);


            //opaque pass
            auto end = std::end(Global::actors);
            for (auto act = std::begin(Global::actors); act != end; ++act) {
#if defined(TRANSPARENCY)
                if (act->getMaterial()->transparency == 0.0f) {
                    DO_ACTOR_JOB(&*act);
                }
                else {
                    passActors.push_back(&(*act));
                }
#else
                DO_ACTOR_JOB(&*act);
#endif


            }

            WAIT_FOR_JOBS();

            //int q = 0;
            auto end0 = std::end(Global::tiles);
            for (auto tile = std::begin(Global::tiles); tile != end0; ++tile) {
                //if(q%2 == 0)
                DO_TILE_JOB(&*tile);
                // q++;
            }

            WAIT_FOR_JOBS();

#if defined(TRANSPARENCY)
            //transparent pass
            auto end2 = std::end(passActors);
            for (auto act = std::begin(passActors); act != end2; ++act) {
                DO_ACTOR_JOB(&**act);
            }

            WAIT_FOR_JOBS();

            end0 = std::end(Global::tiles);
            for (auto tile = std::begin(Global::tiles); tile != end0; ++tile) {
                DO_TILE_JOB(&*tile);
            }

            WAIT_FOR_JOBS();
#endif

        }

        static void initializeTiles() {
            int verts = 1;
            auto end = std::end(Global::actors);
            for (auto act = std::begin(Global::actors); act != end; ++act) {
                verts += act->getVertices()->size();
            }

            for (int x = 0; x < Config::numberOfTilesX; x++) {
                for (int y = 0; y < Config::numberOfTilesY; y++) {
                    Global::tiles[x].pmin = Point(x * Config::tileWidth, y * Config::tileHeight);
                    Global::tiles[x].pmax = Point(x * Config::tileWidth + Config::tileWidth, y * Config::tileHeight + Config::tileHeight);
                    Global::tiles[x].assignedTriangles.clear();

                    for (int i = 0; i < verts; i++) {
                        Global::tiles[x].assignedTriangles.push_back(0);
                    }
                    Global::tiles[x].vectorSize = Global::tiles[x].assignedTriangles.size();
                }
            }
        }

    private:
        static float cosCamX, sinCamX;
        static float cosCamY, sinCamY;
        static float cosCamZ, sinCamZ;
        static CharEngine::Camera* currentCamera;

        static void waitForJobFun() {
            volatile int antiOptimizer = 0;
            while (!Global::pool.isFinished()) { antiOptimizer++; }
        }

        static void submitActor(Actor* actor) {
            vector<Triangle>* const tria = actor->getTriangles();
            auto type = actor->getActorType();
            TranslateMesh(actor->getVertices(), actor->getPosition(), type);

            auto end = std::end(*tria);
            for (auto tri = std::begin(*tria); tri != end; ++tri) {
                prepareTriangle(*tri, actor->getVertices(), actor->getMaterial(), type);
            }
        }

        static void renderTile(Tile* tile) {
            Screen::ClearTilePixels(tile);
            const int len = tile->aT_len;
            Triangle triangle;
            for (int i = 0; i < len; i++) {
                triangle = *(tile->assignedTriangles[i]);
                Screen::DrawTriangle(triangle, tile, &(Global::textures[(triangle).materialp->textureId].textureData));
            }
        }

        static void emptyTiles() {
            auto endt = std::end(Global::tiles);
            for (auto tile = std::begin(Global::tiles); tile != endt; ++tile) {
                tile->aT_len = 0;
            }
        }


        static void prepareTriangle(Triangle& tri, vector<Vertex>* vertices, Material* mat, ActorType* type) {
            //Triangle tri;
            tri.v0 = (*vertices)[tri.v0i].transformed;
            tri.v1 = (*vertices)[tri.v1i].transformed;
            tri.v2 = (*vertices)[tri.v2i].transformed;

            float factor = 255.0f / currentCamera->farMax;
            if (*type != ActorType::Skybox) {
                tri.v0.z = (tri.v0.z + currentCamera->dist) * factor;
                tri.v1.z = (tri.v1.z + currentCamera->dist) * factor;
                tri.v2.z = (tri.v2.z + currentCamera->dist) * factor;
            }
            else {
                tri.v0.z *= factor;
                tri.v1.z *= factor;
                tri.v2.z *= factor;
            }

            //clipping plane intersection
            if ((tri.v0.z <= currentCamera->clipNear || tri.v1.z <= currentCamera->clipNear || tri.v2.z <= currentCamera->clipNear) || 
                tri.v0.z > currentCamera->farMax || tri.v1.z > currentCamera->farMax || tri.v2.z > currentCamera->farMax)//((tri.v0.z <= clipNear && tri.v1.z <= clipNear && tri.v2.z <= clipNear) || tri.v0.z > farMax || tri.v1.z > farMax || tri.v2.z > farMax)
                return;

            /*if (tri.v0.z <= clipNear && tri.v1.z > clipNear && tri.v2.z > clipNear) {
                tri.v0.x = 1000.0f;//(tri.v1.z * tri.v0.x - tri.v0.z * tri.v1.x) / (tri.v1.z - tri.v0.z);
                tri.v0.y = 10.0f;//(tri.v1.z * tri.v0.y - tri.v0.z * tri.v1.y) / (tri.v1.z - tri.v0.z);
                tri.v0.z = 100.0f;//0.001f;
            }*/
            //====

            Screen::PosToScreenCenter(&(tri.v0));
            Screen::PosToScreenCenter(&(tri.v1));
            Screen::PosToScreenCenter(&(tri.v2));

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
            tri.type = *type;

            //bounding box
            tri.bbmax = Point(0, 0);
            tri.bbmin = Point(Config::width - 1, Config::height - 1);
            Vector3* points[3] = { &tri.v0, &tri.v1, &tri.v2 };
            for (int i = 0; i < 3; ++i) {
                if (points[i]->x < tri.bbmin.x)
                    tri.bbmin.x = points[i]->x;
                if (points[i]->y < tri.bbmin.y)
                    tri.bbmin.y = points[i]->y;
                if (points[i]->x > tri.bbmax.x)
                    tri.bbmax.x = points[i]->x;
                if (points[i]->y > tri.bbmax.y)
                    tri.bbmax.y = points[i]->y;
            }
            if (tri.bbmax.x > Config::width - 1) tri.bbmax.x = Config::width - 1;
            if (tri.bbmax.y > Config::height - 1) tri.bbmax.y = Config::height - 1;
            if (tri.bbmin.x < 0) tri.bbmin.x = 0;
            if (tri.bbmin.y < 0) tri.bbmin.y = 0;
            //SetPixelColor(&bbmin, &Color(0, 255, 0));
            //SetPixelColor(&bbmax, &Color(255, 0, 0));

            //single triangle binning
            int t = tri.bbmax.x / Config::tileWidth;
            t = (t > Config::numberOfTilesX - 1) ? Config::numberOfTilesX - 1 : t;
            for (int x = tri.bbmin.x / Config::tileWidth; x <= t; x++) {
                Global::tiles[x].TS_addTriangle(&tri);
            }
        }

        static void setRotation(Vector3 angle) {
            cosCamX = cos(angle.x);
            cosCamY = cos(angle.y);
            cosCamZ = cos(angle.z);
            sinCamX = sin(angle.x);
            sinCamY = sin(angle.y);
            sinCamZ = sin(angle.z);
        }

        static void rotateWorld(float x, float y, float z, Vector3* vec) {
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

        static Vector3 TdToScreen(float x, float y, float z, float cdist) {
            Vector3 rotated(0, 0, 0);
            rotateWorld(x, y, z, &rotated);

            float multiplier = -(0.5f * Config::width / ((rotated.z + cdist) * currentCamera->fov));//-fabsf
            rotated.x *= multiplier;
            rotated.y *= multiplier;

            return std::move(rotated);
        }

        static void TranslateMesh(std::vector<Vertex>* vertices, Vector3* position, ActorType* atype) {
            auto end = std::end(*vertices);
            if (*atype == ActorType::Common) {
                for (auto vertex = std::begin(*vertices); vertex != end; ++vertex) {
                    vertex->transformed = TdToScreen(vertex->raw.x + currentCamera->position.x + position->x, vertex->raw.y + currentCamera->position.y + position->y, vertex->raw.z + currentCamera->position.z + position->z, currentCamera->dist);
                }
            }
            else {
                for (auto vertex = std::begin(*vertices); vertex != end; ++vertex) {
                    vertex->transformed = TdToScreen(vertex->raw.x, vertex->raw.y, vertex->raw.z, 0.0f);
                }
            }
        }
    };

    float Renderer::cosCamX = 0, Renderer::sinCamX = 0;
    float Renderer::cosCamY = 0, Renderer::sinCamY = 0;
    float Renderer::cosCamZ = 0, Renderer::sinCamZ = 0;
    CharEngine::Camera* Renderer::currentCamera = 0;
}
