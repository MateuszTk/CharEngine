#pragma once


#include <iostream>
#include <chrono>

#ifdef OPENCV
	#include "opencv2/core.hpp"
	#include "opencv2/imgproc.hpp"
	#include "opencv2/imgcodecs.hpp"
	#include "opencv2/highgui.hpp"
#endif // OPENCV


#ifdef SDL
	#include <SDL.h>
#endif // SDL

#include <cmath>
#include <vector>
#include <string>
#include "ThreadPool.hpp"


#ifdef OPENCV
using namespace cv;
#endif
using namespace std;
namespace CharEngine {
#ifdef SDL
	typedef unsigned char uchar;
#endif // SDL

	namespace Config {
		int width = 1280;
		int height = 720;
		int halfOfWidth = width * 0.5f;
		int halfOfHeight = height * 0.5f;
		
#ifdef SDL
		const int channels = 4;
#endif // SDL
#ifdef OPENCV
		const int channels = 3;
#endif
#ifdef MULTITHREADING
		const int threads = thread::hardware_concurrency() - 1;
		const int numberOfTilesX = threads;
#else
		const int numberOfTilesX = 11;
#endif
		const int numberOfTilesY = 1;
		int tileWidth = width / numberOfTilesX;
		int tileHeight = height / numberOfTilesY;
	}
	

#ifdef OPENCV
#define R_OFFSET 0
#define G_OFFSET 1
#define B_OFFSET 2
#endif // OPENCV

#ifdef SDL
#define R_OFFSET 2
#define G_OFFSET 1
#define B_OFFSET 0
#endif // SDL

	

	typedef std::chrono::high_resolution_clock Clock;

	enum class ActorType {
		Common,
		Skybox
	};

	template <typename T>
	struct Color {
		T R;
		T G;
		T B;
		Color(T r, T g, T b) {
			R = r;
			G = g;
			B = b;
		}
		Color(const Color<uchar>& c) {
			R = c.R;
			G = c.G;
			B = c.B;
		}

		void UpdateC(T r, T g, T b) {
			R = r;
			G = g;
			B = b;
		}

		void operator *=(const T& w) {
			R *= w;
			G *= w;
			B *= w;
		}

		void operator =(const Color& w) {
			R = w.R;
			G = w.G;
			B = w.B;
		}
	};

	struct ColorA {
		uchar R;
		uchar G;
		uchar B;
		float A;
		ColorA(uchar r, uchar g, uchar b, float a) {
			R = r;
			G = g;
			B = b;
			A = a;
		}
		void UpdateC(uchar r, uchar g, uchar b, float a) {
			R = r;
			G = g;
			B = b;
			A = a;
		}
	};

	struct Vector2 {
		float x;
		float y;
		Vector2(float _x, float _y) {
			x = _x;
			y = _y;
		}

		void UpdateV(float _x, float _y) {
			x = _x;
			y = _y;
		}

		void Normalize() {
			float magnitude = sqrt(x * x + y * y);
			x /= magnitude;
			y /= magnitude;
		}
	};

	struct Vector3 {
		float x;
		float y;
		float z;
		Vector3(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}
		void UpdateV(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}
		void Normalize() {
			float magnitude = sqrt(x * x + y * y + z * z);
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}

		void operator *=(const float& a) {
			x *= a;
			y *= a;
			z *= a;
		}
	};

#ifdef SDL
	struct Point {
		int x;
		int y;
		Point(int _x = 0, int _y = 0) {
			x = _x;
			y = _y;
		}

		void UpdateV(int _x, int _y) {
			x = _x;
			y = _y;
		}
	};

	struct Mat {
		int rows = 0;
		int cols = 0;
		int channels_ = 0;
		bool empty_ = true;
		uchar* data;
		int dataSize = 0;

		void initialize(int _rows = 0, int _cols = 0, int _channels = 0) {
			dataSize = _rows * _cols * _channels;
			data = new uchar[dataSize];
			rows = _rows;
			cols = _cols;
			channels_ = _channels;
			if (dataSize > 0) empty_ = true;
		}

		int channels() {
			return channels_;
		}

		bool empty() {
			return empty_;
		}
	};
#endif // SDL

	struct Material {
		std::string name;
		Color<uchar> color = Color<uchar>(255, 255, 255);
		float transparency = 1.0f;
		int textureId;
	};

	struct Triangle {
		int v0i = 0;
		int v1i = 0;
		int v2i = 0;
		Vector3 v0 = Vector3(0, 0, 0);
		Vector3 v1 = Vector3(0, 0, 0);
		Vector3 v2 = Vector3(0, 0, 0);

		Vector2 uv0 = Vector2(0, 0);
		Vector2 uv1 = Vector2(0, 0);
		Vector2 uv2 = Vector2(0, 0);
		Point bbmin;
		Point bbmax;
		Material* materialp;

		Vector3 normal = Vector3(0, 0, 0);
		ActorType type = ActorType::Common;
	};

	struct Vertex {
		Vector3 raw = Vector3(0, 0, 0);
		Vector3 transformed = Vector3(0, 0, 0);
	};

	struct Texture {
		Mat textureData;
		std::string texturePath = "";
	};


	struct Tile {
		Point pmin;
		Point pmax;
		vector<Triangle*> assignedTriangles;
		int aT_len = 0;
		int vectorSize = 0;

		//thread-safe add
		void TS_addTriangle(Triangle* tri) {
			l_lock.lock();
			assignedTriangles[aT_len] = tri;
			aT_len++;
			if (aT_len >= vectorSize) {
				assignedTriangles.push_back(0);
				vectorSize++;
			}
			l_lock.unlock();
		}

	private:
		std::mutex l_lock;

	};
	namespace Global {
		float* depth;
		Mat image;
#ifdef MULTITHREADING
		ThreadPool pool(Config::threads);
#endif

#ifdef SDL
		SDL_Renderer* renderer = NULL;
		SDL_Texture* texture = NULL;
		SDL_Window* window = NULL;
#endif // SDL
	}

	struct Camera {
		Vector3 angle = Vector3(0, 0, 0);
		Vector3 position = Vector3(0, 0, 0);
		float dist = 10.0f;
		float fov = 1.0f;//(float)tan(80 / 2);
		int farMax = 255;
		int clipNear = 0;

		Camera() = default;
		Camera(Vector3 _angle, Vector3 _position) : angle(_angle), position(_position){

		}
	};

	//TODO: clean up this mess (?remove OpenCV support?)
	class Controller {
	public:
		Camera* camera;
		Controller(Camera* _camera) : camera(_camera){
#ifdef OPENCV
			setMouseCallback("Display window", mouse_callback, this);
#endif // OPENCV 
		}

		/*returns pressed key SDL_Scancode,
		returns SDL_SCANCODE_HOME if no key pressed,
		returns SDL_SCANCODE_ESCAPE if user tries to close the window*/
		SDL_Scancode update() {
			SDL_Scancode key = SDL_SCANCODE_HOME;
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_EventType::SDL_MOUSEMOTION) {
					SDL_GetGlobalMouseState(&x, &y);
				}
				if (e.type == SDL_EventType::SDL_MOUSEWHEEL) {
					camera->dist -= (float)e.wheel.y * 0.5f;
				}
				if (e.type == SDL_EventType::SDL_KEYDOWN) {
					key = e.key.keysym.scancode;

					switch (key) {
						case (SDL_SCANCODE_Q):
							camera->position.y += 1;
							break;
						case (SDL_SCANCODE_E):
							camera->position.y -= 1;
							break;
						case (SDL_SCANCODE_W):
							camera->position.z += 1;
							break;
						case (SDL_SCANCODE_S):
							camera->position.z -= 1;
							break;
						case (SDL_SCANCODE_A):
							camera->position.x += 1;
							break;
						case (SDL_SCANCODE_D):
							camera->position.x -= 1;
							break;
						default:
							break;
					}
				}
				//return SDL_SCANCODE_ESCAPE if user tries to close the window
				if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
					key = SDL_SCANCODE_ESCAPE;
				}
			}

			camera->angle.UpdateV(y / 50.0f, x / 50.0f, 0);
			return key;
	}
	private:
		int x = 0, y = 180;

#ifdef OPENCV
		static void mouse_callback(int event, int xm, int ym, int flag, void* param) {
			Camera* _this = static_cast<Camera*>(param);
			if (event == EVENT_MOUSEMOVE) {
				_this->x = xm;
				_this->y = ym;
			}
			else if (event == EVENT_MOUSEWHEEL) {
				if (getMouseWheelDelta(flag) > 0)
					_this->dist -= 0.8f;
				else
					_this->dist += 0.8f;
			}
		}
#endif // OPENCV

#ifdef SDL
		SDL_Event e;
#endif
	};


	class FPScounter {
	public:
		int tick(bool print = true) {
			auto t2 = Clock::now();
			deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			t1 = Clock::now();
			avgDelta += deltaTime;
			int iFrameRate = 0;

			if (fpsDelay >= _samples) {
				fps = to_string((float)_samples * 1000.0f / avgDelta);
				avgDelta = 0;
				fpsDelay = 0;
				if(print) cout << "FPS: " << fps << '\n';
			}
			else fpsDelay++;

			return iFrameRate;
		}

		FPScounter(int samples = 40) {
			t1 = Clock::now();
			_samples = samples;
		}

	private:
		float deltaTime = 0;
		int _samples;
		Clock::time_point t1;
		int fpsDelay = 0;
		string fps;
		float avgDelta = 0;
	};
}
