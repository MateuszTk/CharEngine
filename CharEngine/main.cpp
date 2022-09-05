#include "CharEngine.h"

int main(int argc, char* argv[])
{
    Screen::CreateWindow();

    CharEngine::Camera camera(CharEngine::Camera::Type::MouseControl);

    auto car = objLoader::LoadObj("../data/car");

    Renderer::initializeTiles();
    
    CharEngine::FPScounter fps;

    while (true)
    {
        camera.update();

        Renderer::render(camera);      
        Screen::PrintFrame();
        fps.tick();
    }
    return 0;
}
