#pragma once

#define MULTITHREADING
#define SDL
//#define OPENCV
//#define DISABLE_AVX


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
#include "ThreadPool.h"


#ifdef OPENCV
using namespace cv;
#endif
using namespace std;

#ifdef SDL
typedef unsigned char uchar;
#endif // SDL


const int width = 1280;
const int height = 720;
const int halfOfWidth = width * 0.5f;
const int halfOfHeight = height * 0.5f;
float dist = 16;
const float fov = 1.0f;//(float)tan(80 / 2);
const int farMax = 255;
const int clipNear = 0;
#ifdef SDL
const int channels = 4;
#endif // SDL
#ifdef OPENCV
const int channels = 3;
#endif

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

const int numberOfTilesX = 11;
const int numberOfTilesY = 1;
const int tileWidth = width / numberOfTilesX;
const int tileHeight = height / numberOfTilesY;

enum class ActorType
{
	Common,
	Skybox
};

struct Color
{
	uchar R;
	uchar G;
	uchar B;
	Color(uchar r, uchar g, uchar b)
	{
		R = r;
		G = g;
		B = b;
	}
	void UpdateC(uchar r, uchar g, uchar b)
	{
		R = r;
		G = g;
		B = b;
	}

	void operator *=(const float& w)
	{
		R *= w;
		G *= w;
		B *= w;
	}
};

struct fColor
{
	float R;
	float G;
	float B;
	fColor(float r, float g, float b)
	{
		R = r;
		G = g;
		B = b;
	}

	fColor(Color& c)
	{
		R = c.R;
		G = c.G;
		B = c.B;
	}

	void UpdateC(float r, float g, float b)
	{
		R = r;
		G = g;
		B = b;
	}

	void operator *=(const float& w)
	{
		R *= w;
		G *= w;
		B *= w;
	}

	void operator =(const Color& w)
	{
		R = w.R;
		G = w.G;
		B = w.B;
	}
};


struct ColorA
{
	uchar R;
	uchar G;
	uchar B;
	float A;
	ColorA(uchar r, uchar g, uchar b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
	void UpdateC(uchar r, uchar g, uchar b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
};

struct Vector2
{
	float x;
	float y;
	Vector2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	void UpdateV(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	void Normalize()
	{
		float magnitude = sqrt(x * x + y * y);
		x /= magnitude;
		y /= magnitude;
	}
};

struct Vector3
{
	float x;
	float y;
	float z;
	Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	void UpdateV(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	void Normalize()
	{
		float magnitude = sqrt(x * x + y * y + z * z);
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}

	void operator *=(const float& a)
	{
		x *= a;
		y *= a;
		z *= a;
	}
};

#ifdef SDL
struct Point
{
	int x;
	int y;
	Point(int _x = 0, int _y = 0)
	{
		x = _x;
		y = _y;
	}

	void UpdateV(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
};

struct Mat
{
	int rows = 0;
	int cols = 0;
	int channels_ = 0;
	bool empty_ = true;
	uchar* data;
	int dataSize = 0;

	void initialize(int _rows = 0, int _cols = 0, int _channels = 0)
	{
		dataSize = _rows * _cols * _channels;
		data = new uchar[dataSize];
		rows = _rows;
		cols = _cols;
		channels_ = _channels;
		if (dataSize > 0) empty_ = true;
	}

	int channels()
	{
		return channels_;
	}

	bool empty()
	{
		return empty_;
	}
};
#endif // SDL

struct Material
{
	std::string name;
	Color color = Color(255, 255, 255);
	float transparency = 1.0f;
	int textureId;
};

struct Triangle
{
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

struct pTriangle
{
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	Vector2 uv1 = Vector2(0, 0);
	Vector2 uv2 = Vector2(0, 0);
	Vector2 uv3 = Vector2(0, 0);
	Triangle triangleData;
};

struct Vertex
{
	Vector3 raw = Vector3(0, 0, 0);
	Vector3 transformed = Vector3(0, 0, 0);
};

struct Texture
{
	Mat textureData;
	std::string texturePath = "";
};


struct Tile
{
	Point pmin;
	Point pmax;
	vector<Triangle*> assignedTriangles;
	int aT_len = 0;

	//thread-safe add
	void TS_addTriangle(Triangle* tri)
	{
		l_lock.lock();
		assignedTriangles[aT_len] = tri;
		aT_len++;
		l_lock.unlock();
	}

private:
	std::mutex l_lock;

};

extern float depth[width][height];
extern Mat image;
extern Vector3 cameraAngle;
extern Vector3 cameraPosition;
extern float deltaTime;
extern ThreadPool pool;

#ifdef SDL
extern SDL_Renderer* renderer;
extern SDL_Texture* texture;
extern SDL_Window* window;
#endif // SDL

