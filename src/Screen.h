#pragma once

/*#include <iostream>
#include <windows.h>
#include <chrono>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cmath>*/
#include "Helper.h"



//using namespace helper;
using namespace cv;


class Screen
{
public:
	static Point* PosToScreenCenter(Point* pos)
	{
		float yResize = (float)height / (float)width;
		pos->x = halfOfWidth + pos->x;
		pos->y = (halfOfHeight - pos->y) * yResize;
		return pos;
	}

	static Vector3* PosToScreenCenter(Vector3* pos)
	{
		float yResize = (float)height / (float)width;
		pos->x = halfOfWidth + pos->x;
		pos->y = (halfOfHeight - pos->y) * yResize;
		return pos;
	}

	static void SetPixelColor(Point* point, Color* color)
	{
		//PosToScreenCenter(&point);
		image.data[point->y * width * channels + point->x * channels] = color->B;
		image.data[point->y * width * channels + point->x * channels + 1] = color->G;
		image.data[point->y * width * channels + point->x * channels + 2] = color->R;
	}

	static void MixPixelColor(Point* point, ColorA* color)
	{
		//PosToScreenCenter(&point);
		image.data[point->y * width * channels + point->x * channels] = (uchar)(((float)(image.data[point->y * width * channels + point->x * channels]) - color->B) * color->A + color->B);
		image.data[point->y * width * channels + point->x * channels + 1] = (uchar)(((float)(image.data[point->y * width * channels + point->x * channels + 1]) - color->G) * color->A + color->G);
		image.data[point->y * width * channels + point->x * channels + 2] = (uchar)(((float)(image.data[point->y * width * channels + point->x * channels + 2]) - color->R) * color->A + color->R);
	}

	static void PrintFrame()
	{
		imshow("Display window", image);
		//waitKey(1);
		image.setTo(Scalar(140, 80, 10));
		fill_n(&depth[0][0], width * height, 255);
	}

	static void DebugDepth()
	{
		Point p;
		Color c(0, 0, 0);
		for (int x = 0; x < image.cols; x++)
		{
			for (int y = 0; y < image.rows; y++)
			{
				p.x = x;
				p.y = y;
				c.B = c.G = c.R = depth[y][x];
				SetPixelColor(&p, &c);
			}
		}
		imshow("Display window", image);
		waitKey(1);
		image.setTo(Scalar(0, 0, 0));
		fill_n(&depth[0][0], width * height, 255);
	}

	static void DrawTriangle(Vector3 v0, Vector3 v1, Vector3 v2, Color c0, Color c1, Color c2, float transparency)
	{
		if (v0.z <= clipNear || v1.z <= clipNear || v2.z <= clipNear || v0.z > farMax || v1.z > farMax || v2.z > farMax)
			return;

		PosToScreenCenter(&v0);
		PosToScreenCenter(&v1);
		PosToScreenCenter(&v2);

		//face normal check
		Vector3 a(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z); //v1 - v0
		Vector3 b(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z); //v2 - v0
		Vector3 normal = Vector3(0, 0, a.x * b.y - a.y * b.x);
		
		if (normal.z < 0)
		{
			//bounding box
			Point bbmax(0, 0);
			Point bbmin(width - 1, height - 1);
			Vector3* points[3] = { &v0, &v1, &v2 };
			for (int i = 0; i < 3; ++i)
			{
				if (points[i]->x < bbmin.x)
					bbmin.x = points[i]->x;
				if (points[i]->y < bbmin.y)
					bbmin.y = points[i]->y;
				if (points[i]->x > bbmax.x)
					bbmax.x = points[i]->x;
				if (points[i]->y > bbmax.y)
					bbmax.y = points[i]->y;
			}
			if (bbmax.x > width - 1) bbmax.x = width - 1;
			if (bbmax.y > height - 1) bbmax.y = height - 1;
			if (bbmin.x < 0) bbmin.x = 0;
			if (bbmin.y < 0) bbmin.y = 0;
			//SetPixelColor(&bbmin, &Color(0, 255, 0));
			//SetPixelColor(&bbmax, &Color(255, 0, 0));


			//face specularity effect
			normal.x = a.y * b.z - a.z * b.y;
			normal.y = a.z * b.x - a.x * b.z;
			normal.Normalize();
			float specularity = (1.0f + normal.z) * 120.0f + 0.8f;
			specularity = (specularity > 1.0f) ? 1.0f : ((specularity < 0.0f) ? 0.0f : specularity);

			Vector2 pointVector(0, 0);
			Vector2 vect(0, 0);
			bool inside = true;
			bool end = false;
			float e1, e2, e3;
			float area = (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
			float depthl;
			Point p1;
			ColorA cx(0, 0, 0, transparency);
			c0 *= specularity;
			c1 *= specularity;
			c2 *= specularity;

			for (int x = bbmin.x; x <= bbmax.x; x++)
			{
				end = false;
				for (int y = bbmin.y; y <= bbmax.y; y++)
				{

					inside = true;
					//checking if the pixel is inside the triangle (cross product)
					inside = (e3 = (x - v0.x) * (v1.y - v0.y) - (y - v0.y) * (v1.x - v0.x)) >= 0; 
					if (!inside)
						continue;

					inside = (e1 = (x - v1.x) * (v2.y - v1.y) - (y - v1.y) * (v2.x - v1.x)) >= 0;
					if (!inside)
						continue;

					inside = (e2 = (x - v2.x) * (v0.y - v2.y) - (y - v2.y) * (v0.x - v2.x)) >= 0;

					if (inside)
					{
						e1 /= area;
						e2 /= area;
						e3 /= area;
						depthl = e1 * v0.z + e2 * v1.z + e3 * v2.z;
						if (depth[y][x] > depthl)
						{
							p1.x = x;
							p1.y = y;
							cx.R = (uchar)(e1 * c0.R + e2 * c1.R + e3 * c2.R);
							cx.G = (uchar)(e1 * c0.G + e2 * c1.G + e3 * c2.G);
							cx.B = (uchar)(e1 * c0.B + e2 * c1.B + e3 * c2.B);

							MixPixelColor(&p1, &cx);
							depth[y][x] = depthl;
							end = true;
						}
					}
					else if (end)
					{
						break;
					}
				}
			}
		}
	}
};

