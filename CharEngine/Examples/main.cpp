#define MULTITHREADING
#define SDL
//#define OPENCV
//#define DISABLE_AVX
#define TRANSPARENCY

#include "../CharEngine/CharEngine.hpp"

int main(int argc, char* argv[])
{
    CharEngine::Screen::CreateWindow(1280, 720);

    CharEngine::Camera camera(CharEngine::Camera::Type::MouseControl);

    auto car = CharEngine::ObjLoader::LoadObj("../assets/car/car");

    CharEngine::Renderer::initializeTiles();
    
    CharEngine::FPScounter fps;

    while (true)
    {
        camera.update();

        CharEngine::Renderer::render(camera);
        CharEngine::Screen::PrintFrame();
        fps.tick();
    }
    return 0;
}
