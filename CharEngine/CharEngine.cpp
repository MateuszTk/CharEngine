#include "Helper.h"
#include "Screen.h"
#include "Renderer.h"
#include "Main.h"


//using namespace helper;
using namespace cv;

typedef std::chrono::high_resolution_clock Clock;

Vector3 cameraAngle = Vector3(0, 0, 0);
Vector3 cameraPosition = Vector3(0, 0, 0);
float depth[width][height];
//float normals[width][height];
Mat image;
float deltaTime = 0;

#ifdef MULTITHREADING
// Create threads
ThreadPool pool(3);
#endif

int x = 0, y = 0;

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
            dist -= 0.2f;
        else
            dist += 0.2f;
    }
}

int main()
{

    //std::cout << __cplusplus << std::endl;

    namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
    image = Mat::zeros(height, width, CV_8UC3);

    auto t1 = Clock::now();
    setMouseCallback("Display window", mouse_callback);

    Main mainObj;
    mainObj.Start();

    uchar fpsDelay = 0;
    string fps;
    float avgDelta = 0;

    while (true)
    {

        cameraAngle.y =  x / 50.0f;
        cameraAngle.x =  y / 50.0f;

        char key = (char)waitKey(1);

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

        if (fpsDelay >= 10)
        {           
            fps = to_string(10000.0f / avgDelta) + " FPS x" + to_string(x) + " y " + to_string(y);     
            avgDelta = 0;           
            fpsDelay = 0;
        }
        else
            fpsDelay++;

        Renderer::render();
        cv::putText(image, fps, Point(0, 12), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(255, 255, 255), 1);
        Screen::PrintFrame();
        //Screen::DebugDepth();
        //Screen::DebugNormals();
    }
    return 0;
}