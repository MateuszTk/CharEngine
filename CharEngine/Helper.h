#pragma once

#include <iostream>
#include <chrono>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cmath>
#include <vector>
#include "ThreadPool.h"

#define AVX
#define MULTITHREADING

using namespace cv;
using namespace std;

const int width = 1280;
const int height = 720;
const int halfOfWidth = width * 0.5f;
const int halfOfHeight = height * 0.5f;
float dist = 8;
const float fov = (float)tan(80 / 2);
const int farMax = 255;
const int clipNear = 1;
const int channels = 3;


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

struct Triangle
{
	Vector3 v1 = Vector3(0, 0, 0);
	Vector3 v2 = Vector3(0, 0, 0);
	Vector3 v3 = Vector3(0, 0, 0);
	Color vertexColor1 = Color(255, 255, 255);
	Color vertexColor2 = Color(255, 255, 255);
	Color vertexColor3 = Color(255, 255, 255);
	Vector2 uv1 = Vector2(0, 0);
	Vector2 uv2 = Vector2(0, 0);
	Vector2 uv3 = Vector2(0, 0);
};

struct pTriangle
{
	int v1;
	int v2;
	int v3;
	Vector2 uv1 = Vector2(0, 0);
	Vector2 uv2 = Vector2(0, 0);
	Vector2 uv3 = Vector2(0, 0);
	/*int uv_v1 = -1;
	int uv_v2 = -1;
	int uv_v3 = -1;*/
};

struct Vertex
{
	Vector3 raw = Vector3(0, 0, 0);
	Vector3 transformed = Vector3(0, 0, 0);
};

struct Material
{
	std::string name;
	Color color = Color(255, 255, 255);
	float transparency = 1.0f;
	Mat texture;
};



extern float depth[width][height];
//extern float normals[width][height];
extern Mat image;
extern Vector3 cameraAngle;
extern Vector3 cameraPosition;
extern float deltaTime;
extern ThreadPool pool;

