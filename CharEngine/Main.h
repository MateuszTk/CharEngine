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

    vector<Actor*> bridge0a;
    vector<Actor*> bridge1a;
    vector<Actor*> bridge2a;

    // Use this for initialization
    void Start() override
    {
        auto car = objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\car");
        /*auto bridge0 = objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\bridge");
        auto bridge1 = objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\bridge", "1");
        auto bridge2 = objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\bridge", "2");

        bridge0a = Actor::Find(&bridge0, &actors);
        bridge1a = Actor::Find(&bridge1, &actors);
        bridge2a = Actor::Find(&bridge2, &actors);*/

        /*Material mat;
        Vector3 pos(0,0,0);
        mat.color.UpdateC(0, 255, 255);
        mat.transparency = 1;*/

        /*for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                pos.UpdateV(x * 2 - 1, 0, y * 2 - 1);
                objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\cube", "", pos);
            }
        }*/
        //pos.UpdateV(0, 0, 0);
        //objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\cube", "", pos);
    }

    //Vector3 cpos = Vector3(0,0,0);

    // Update is called once per frame
    void Update() override
    {
       /* float speed = 1;
        cpos.x = 8000;//speed * deltaTime;
        int d = 84 * 100;
        int segments = 3;

        Vector3 pos((d - ((int)(cpos.x + d/ segments)% d) - d/ segments) / 100.0f, 0, 0);
        Actor::SetGroupPosition(&bridge0a, &pos);
        pos.UpdateV((d - ((int)cpos.x % d) - d/ segments) / 100.0f, 0, 0 );
        Actor::SetGroupPosition(&bridge1a, &pos);
        pos.UpdateV((d - ((int)(cpos.x - d/ segments) % d) - d/ segments) / 100.0f, 0, 0);
        Actor::SetGroupPosition(&bridge2a, &pos);*/
    }
};
