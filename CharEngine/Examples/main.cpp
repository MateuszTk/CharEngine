#define MULTITHREADING
#define SDL
//#define OPENCV
//#define DISABLE_AVX
#define TRANSPARENCY

#include "../CharEngine/CharEngine.hpp"

#ifdef __linux__
void print_cpu_freq(std::ofstream& log) {
    std::ifstream file("/proc/cpuinfo");
    std::string str;
    std::vector<float> freqs;
    while (std::getline(file, str)) {
        if (str.find("cpu MHz") != std::string::npos) {
            freqs.push_back(std::stof(str.substr(str.find(":") + 2)));
        }
    }
    for (auto f : freqs) {
        log << f << " ";
    }
    std::cout << "    Max freq: " << *std::max_element(freqs.begin(), freqs.end()) << std::endl;
    //std::cout << "Min freq: " << *std::min_element(freqs.begin(), freqs.end()) << std::endl;
    std::cout << "    Avg freq: ";
    float sum = 0;
    for (auto f : freqs) {
        sum += f;
    }
    std::cout << sum / freqs.size() << "\n\n";
}
#endif

int main(int argc, char* argv[])
{
    // test modes:
    // full
    // simd_off
    // simd_on

    // read from argv
    std::string mode = "full";
    if (argc > 1) {
        mode = argv[1];
        // remove following dashes
        if (mode[0] == '-') {
            mode = mode.substr(1);
        }
        std::cout << "Mode: " << mode << '\n';
    }

    if (mode == "simd_off") {
        CharEngine::Config::useAVX = false;
        std::cout << "AVX OFF\n";
    }
    else if (mode == "simd_on") {
        CharEngine::Config::useAVX = true;
        std::cout << "AVX ON\n";
    }
    else{
        std::cout << "Option not recognized, using full mode\n";
        mode = "full";
    }

    std::ofstream file("log_" + mode + ".txt");

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

    CharEngine::Global::pool.resize(1);

    int threads = 0;
    fps.addCallback([&](float time, int frames) {
        if(threads == 0) {
            std::cout << "Warmup done\n";
        }
        else{
            float fps = frames * 1000.0f / time;
            std::cout << "Threads: " << threads << " FPS: " << fps  << std::endl;
            file << fps << " ";
            #ifdef __linux__
            print_cpu_freq(file);
            #endif
            file << std::endl;
        }
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
            if (loop == 0 && mode == "full") {
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
    file.close();
    return 0;
}
