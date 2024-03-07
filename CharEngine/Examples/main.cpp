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

int main(int argc, char* argv[]) {
    // read from argv
    std::string mode = "auto";
    int threads = 1;
    while (argc > 1) {
        std::string arg = argv[argc - 1];

        // decapitalize
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

        if (arg.find("mode=") != std::string::npos) {
			mode = arg.substr(arg.find("=") + 1);
		}
        else if (arg.find("threads=") != std::string::npos) {
            threads = std::stoi(arg.substr(arg.find("=") + 1));
            threads = std::max(1, std::min(threads, 1000));
		}
		else if (arg.find("simd=") != std::string::npos) {
            std::string avxBool = arg.substr(arg.find("=") + 1);
            if (avxBool == "on" || avxBool == "true") {
				CharEngine::Config::useAVX = true;
				std::cout << "AVX ON\n";
			}
			else if (avxBool == "off" || avxBool == "false") {
				CharEngine::Config::useAVX = false;
				std::cout << "AVX OFF\n";
			}
		}
        else if (arg.find("help") != std::string::npos) {
			std::cout << "Usage: " << argv[0] << " [mode=auto|manual] [threads=1-1000] [simd=on|off]\n";
			return 0;
		}
        else {
            std::cout << "Option '" << arg << "' not recognized\n";
        }

        argc--;
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
    const int frames = 500;
    CharEngine::FPScounter fps(frames);

    CharEngine::Global::pool.resize(threads);

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
        if (mode == "auto") {
            threads++;
            CharEngine::Global::pool.resize(threads);
        }
	});

    int loop = 0;

    while (true)
    {
        SDL_Scancode key = camController.update();

        CharEngine::Renderer::render(camera);
        CharEngine::Screen::PrintFrame();

        fps.tick(false);

        camController.rotate((2.0f * M_PI) / frames, 0.0f);

        if (threads > 20 && mode == "auto") {
            if (loop == 0 && mode == "auto") {
                CharEngine::Config::useAVX = !CharEngine::Config::useAVX;
                loop++;
                threads = 1;
                std::cout << "AVX: " << (CharEngine::Config::useAVX ? "ON" : "OFF") << std::endl;
                CharEngine::Global::pool.resize(threads);
            }
            else {
                std::cout << "End of test\n";
                break;
            }
        }

        if (key == SDL_SCANCODE_ESCAPE) break;
    }
    file.close();
    return 0;
}
