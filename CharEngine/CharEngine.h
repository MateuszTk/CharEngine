#pragma once

#include "Helper.h"
#include "Screen.h"
#include "Renderer.h"
#include "Actor.h"
#include "ObjInterpreter.h"


float depth[width * height];
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
