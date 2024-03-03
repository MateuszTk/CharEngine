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
    CharEngine::Controller camController(&camera, false);
    camera.dist = 5;

    //Create Actor by loading ".obj" files
    //Important: ".obj" file extension must be omnited, as the same path is used to load ".mtl" files
    auto car = CharEngine::ObjLoader::LoadObj("../assets/car/car");
    //Prepare Actor for rendering; This function MUST be called every time after loading new Actors
    CharEngine::Renderer::initializeTiles();
    
    //(Optional) Initialize framerate counter
    CharEngine::FPScounter fps(500);

    int threads = 0;
    fps.addCallback([&](float time, int frames) {
        std::cout << "Threads: " << threads << " FPS: " << (frames * 1000.0f / time)  << std::endl;
        threads++;
        CharEngine::Global::pool.resize(threads);
	});

    int loop = 0;

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
        fps.tick(false);
        if (threads > 20) { 
            if (loop == 0) {
                std::cout << "AVX OFF\n";
                CharEngine::Config::useAVX = false;
                loop++;
                threads = 1;
                CharEngine::Global::pool.resize(threads);
            }
            else {
                std::cout << "End of test\n";
                break;
            }
        }

        //Exit by pressing ESCAPE
        if (key == SDL_SCANCODE_ESCAPE) break;
    }
    return 0;
}
