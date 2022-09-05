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
        auto car = objLoader::LoadObj("../data/car");
    }

    // Update is called once per frame
    void Update() override
    {

    }
};
