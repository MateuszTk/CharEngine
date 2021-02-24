#pragma once
#include <string>
#include "Helper.h"
#include "Screen.h"
#include "Renderer.h"
#include "Actor.h"
#include "ObjInterpreter.h"

class Main : public Renderer
{
public:

    //vector<Actor*> carv;

    // Use this for initialization
    void Start() override
    {
        int d = 100;
        //skybox must be loaded first
        /*objLoader::LoadObj("./data/skybox0", "0", Vector3(0, 0, 0), ActorType::Skybox);
        auto car = objLoader::LoadObj("./data/car");
        objLoader::LoadObj("./data/cybercity88", "0", Vector3(0, 0, 0));
        objLoader::LoadObj("./data/cybercity88", "1", Vector3(-d, 0, 0));
        objLoader::LoadObj("./data/cybercity88", "2", Vector3(d, 0, 0));

        objLoader::LoadObj("./data/cybercity88", "0", Vector3(0, 0, -d));
        objLoader::LoadObj("./data/cybercity88", "1", Vector3(-d, 0, -d));
        objLoader::LoadObj("./data/cybercity88", "2", Vector3(d, 0, -d));

        objLoader::LoadObj("./data/cybercity88", "0", Vector3(0, 0, d));
        objLoader::LoadObj("./data/cybercity88", "1", Vector3(-d, 0, d));
        objLoader::LoadObj("./data/cybercity88", "2", Vector3(d, 0, d));*/

        //carv = Actor::Find(&car, &actors);
       

        Material mat;
        Vector3 pos(0,0,0);
        mat.color.UpdateC(0, 255, 255);
        mat.transparency = 1;

        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                pos.UpdateV(x * 2 - 1, 0, y * 2 - 1);
                objLoader::LoadObj("../data/cube", "", pos);
            }
        }
        pos.UpdateV(0, 2, 0);
        objLoader::LoadObj("../data/cube", "", pos);
    }

    //float s = 0;

    // Update is called once per frame
    void Update() override
    {
        /*float speed = 0.001f;
        s += speed * deltaTime;
        float x = sin(s) * 10;

        auto end = std::end(carv);
        for (auto act = std::begin(carv); act != end; ++act)
        {
            (*act)->getPosition()->UpdateV(x, 0, 0);
        }*/
        //cameraPosition.UpdateV(-x, 0, 0);
        /*int d = 84 * 100;
        int segments = 3;

        Vector3 pos((d - ((int)(cpos.x + d/ segments)% d) - d/ segments) / 100.0f, 0, 0);
        Actor::SetGroupPosition(&bridge0a, &pos);
        pos.UpdateV((d - ((int)cpos.x % d) - d/ segments) / 100.0f, 0, 0 );
        Actor::SetGroupPosition(&bridge1a, &pos);
        pos.UpdateV((d - ((int)(cpos.x - d/ segments) % d) - d/ segments) / 100.0f, 0, 0);
        Actor::SetGroupPosition(&bridge2a, &pos);*/
    }
};
