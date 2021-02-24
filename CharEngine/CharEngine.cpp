#include "Helper.h"
#include "Screen.h"
#include "Renderer.h"
#include "Main.h"

typedef std::chrono::high_resolution_clock Clock;

Vector3 cameraAngle = Vector3(0, 0, 0);
Vector3 cameraPosition = Vector3(0, 0, 0);
float depth[width][height];
Mat image;
float deltaTime = 0;
#ifdef SDL
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Window* window = NULL;
#endif // SDL


#ifdef MULTITHREADING
// Create threads
const int threads = thread::hardware_concurrency() - 1;
ThreadPool pool(threads);
#endif

int x = 0, y = 180;
#ifdef SDL
SDL_Event e;
#endif
#ifdef OPENCV
void mouse_callback(int event, int xm, int ym, int flag, void* param)
{
    if (event == EVENT_MOUSEMOVE)
    {
        x = xm;
        y = ym;
    }
    else if (event == EVENT_MOUSEWHEEL)
    {
        if (getMouseWheelDelta(flag) > 0)
            dist -= 0.8f;
        else
            dist += 0.8f;
    }
}
#endif // OPENCV

#ifdef SDL
void updateMousePosition()
{
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_MOUSEMOTION)
        {
            SDL_GetGlobalMouseState(&x, &y);
        }
    }
}
#endif

int main(int argc, char* argv[])
{

    //std::cout << __cplusplus << std::endl;
    Screen::CreateWindow();

    auto t1 = Clock::now();
#ifdef OPENCV
    setMouseCallback("Display window", mouse_callback);
#endif // OPENCV 

    Texture emptyTexture;
    textures.push_back(emptyTexture);

    Main mainObj;
    mainObj.Start();

    uchar fpsDelay = 0;
    string fps;
    float avgDelta = 0;

    Renderer::initializeTiles();

    while (true)
    {
#ifdef SDL
        updateMousePosition();
#endif
        cameraAngle.y =  x / 50.0f;
        cameraAngle.x =  y / 50.0f;
        char key = ' ';

#ifdef OPENCV
        key = (char)waitKey(1);
#endif // OPENCV 
        switch (key)
        {
        case ('q'):
            cameraPosition.y += 1;
            break;
        case ('e'):
            cameraPosition.y -= 1;
            break;
        case ('w'):
            cameraPosition.z += 1;
            break;
        case ('s'):
            cameraPosition.z -= 1;
            break;
        case ('a'):
            cameraPosition.x += 1;
            break;
        case ('d'):
            cameraPosition.x -= 1;
            break;
        }
        mainObj.Update();


        auto t2 = Clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        t1 = Clock::now();
        avgDelta += deltaTime;

        if (fpsDelay >= 20)
        {           
            fps = to_string(20000.0f / avgDelta) + " FPS x" + to_string(x) + " y " + to_string(y);     
            avgDelta = 0;           
            fpsDelay = 0;
        }
        else
            fpsDelay++;
        
        Renderer::render();
#ifdef OPENCV
        cv::putText(image, fps, Point(0, 12), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
#endif // OPENCV  
#ifdef SDL
        cout << "FPS: " << fps << '\n';
#endif // SDL

        Screen::PrintFrame();
        //Screen::DebugDepth();
        //Screen::DebugNormals();
    }
    return 0;
}
