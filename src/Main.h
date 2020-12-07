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

    // Use this for initialization
    void Start() override
    {
        //auto car = objLoader::LoadObj("F:\\dokumenty\\obiekty\\lowpoly\\car");

        Color* c1 = &Color(255, 0, 0);
        Color* c2 = &Color(0, 255, 0);
        Color* c3 = &Color(0, 0, 255);
        Vector3 pos(0,0,0);

        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                pos.UpdateV(x * 2 - 1, 0, y * 2 - 1);
                actors.push_back(Actor::createCube(pos, c1, c2, c3));
            }
        }
        pos.UpdateV(0, -2, 0);
        actors.push_back(Actor::createCube(pos, c1, c2, c3));
    }

    // Update is called once per frame
    void Update() override
    {
        //actors[0].getPosition()->x += 0.01f;
    }
};
