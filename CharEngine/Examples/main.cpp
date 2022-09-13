#define MULTITHREADING
#define SDL
//#define OPENCV
//#define DISABLE_AVX
#define TRANSPARENCY

#include "../CharEngine/CharEngine.hpp"

int main(int argc, char* argv[])
{
    //Create window
    CharEngine::Screen::CreateWindow(800, 600);

    //Create a camera and its controller
    CharEngine::Camera camera(CharEngine::Vector3(0,0,0), CharEngine::Vector3(0,-1,0));
    CharEngine::Controller camController(&camera);

    //Create Actor by loading ".obj" files
    //Important: ".obj" file extension must be omnited, as the same path is used to load ".mtl" files
    auto car = CharEngine::ObjLoader::LoadObj("../../assets/car/car");
    //Prepare Actor for rendering; This function MUST be called every time after loading new Actors
    CharEngine::Renderer::initializeTiles();
    
    //(Optional) Initialize framerate counter
    CharEngine::FPScounter fps;

    while (true)
    {
        //Allows to move and rotate camera around with mouse and keyboard; 
        //Additionally returns pressed keyboard key (SDL_SCANCODE_HOME if none) 
        SDL_Scancode key = camController.update();

        //Render scene to the buffer
        CharEngine::Renderer::render(camera);
        //Displays buffer on the screen
        CharEngine::Screen::PrintFrame();

        //Calculate framerate
        //Optional argument controls whether FPS should be printed to the terminal
        fps.tick();
        //Exit by pressing ESCAPE
        if (key == SDL_SCANCODE_ESCAPE) break;
    }
    return 0;
}
