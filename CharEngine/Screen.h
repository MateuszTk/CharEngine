#pragma once

#include "Helper.h"
#include "AVX.h"


//#define SMART_AVX

//using namespace helper;
using namespace cv;

class Screen
{

public:
	static Point* PosToScreenCenter(Point* pos)
	{
		pos->x = halfOfWidth + pos->x;
		pos->y = halfOfHeight - pos->y;
		return pos;
	}

	static Vector3* PosToScreenCenter(Vector3* pos)
	{
		pos->x = halfOfWidth + pos->x;
		pos->y = halfOfHeight - pos->y;
		return pos;
	}

	static void SetPixelColor(Point* point, ColorA* color)
	{
		//PosToScreenCenter(&point);
		int i = point->y * width * channels + point->x * channels;
		image.data[i] = color->B;
		image.data[i + 1] = color->G;
		image.data[i + 2] = color->R;
	}

	static void MixPixelColor(Point* point, ColorA* color)
	{
		//PosToScreenCenter(&point);
		int i = point->y * width * channels + point->x * channels;
		image.data[i] = (uchar)(((image.data[i]) - color->B) * color->A + color->B);
		image.data[i + 1] = (uchar)(((image.data[i + 1]) - color->G) * color->A + color->G);
		image.data[i + 2] = (uchar)(((image.data[i + 2]) - color->R) * color->A + color->R);
	}

	static void idSetPixelColor(int id, ColorA* color)
	{
		//int i = point->y * width * channels + point->x * channels;
		image.data[id] = color->B;
		image.data[id + 1] = color->G;
		image.data[id + 2] = color->R;
	}

	static void idMixPixelColor(int id, ColorA* color)
	{
		//int i = point->y * width * channels + point->x * channels;
		image.data[id] = (uchar)(((image.data[id]) - color->B) * color->A + color->B);
		image.data[id + 1] = (uchar)(((image.data[id + 1]) - color->G) * color->A + color->G);
		image.data[id + 2] = (uchar)(((image.data[id + 2]) - color->R) * color->A + color->R);
	}

	static void GetPixelColor(Point* point, ColorA* color, int cn, Mat* im)
	{
		//PosToScreenCenter(&point);
		int i = point->y * im->rows * cn + point->x * cn;
		color->B = im->data[i];
		color->G = im->data[i + 1];
		color->R = im->data[i + 2];
	}

	static void idGetPixelColor(int id, ColorA* color, Mat* im)
	{
		color->B = im->data[id];
		color->G = im->data[id + 1];
		color->R = im->data[id + 2];
	}

	static void PrintFrame()
	{
		imshow("Display window", image);
		//waitKey(1);
		image.setTo(Scalar(140, 80, 10));
		fill_n(&depth[0][0], width * height, 255);
		//fill_n(&normals[0][0], width * height, 0);
	}

	void DebugDepth()
	{
		Point p;
		ColorA c(0, 0, 0, 0);
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

	static void DrawTriangle(Vector3 v0, Vector3 v1, Vector3 v2, Color c0b, Color c1b, Color c2b, float transparency, Mat* texture, Vector2 uv0, Vector2 uv1, Vector2 uv2)
	{
		if (v0.z <= clipNear || v1.z <= clipNear || v2.z <= clipNear || v0.z > farMax || v1.z > farMax || v2.z > farMax)
			return;

		PosToScreenCenter(&v0);
		PosToScreenCenter(&v1);
		PosToScreenCenter(&v2);

		fColor c0(c0b);
		fColor c1(c1b);
		fColor c2(c2b);

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

#if defined SMART_AVX
			//true - use AVX
			const bool avxMode = bbmax.y - bbmin.y > 8;
#endif

			//face specularity effect
			normal.x = a.y * b.z - a.z * b.y;
			normal.y = a.z * b.x - a.x * b.z;
			normal.Normalize();
			float specularity = (1.0f + normal.z) * 120.0f + 0.8f;
			specularity = (specularity > 1.0f) ? 1.0f : ((specularity < 0.0f) ? 0.0f : specularity);

			bool inside = true;
			bool end = false;
			
			const float area = (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);

			ColorA cx(0, 0, 0, transparency);
			c0 *= specularity;
			c1 *= specularity;
			c2 *= specularity;
#if !defined AVX || defined SMART_AVX

			void (*pixel_placer)(Point*, ColorA*) = (transparency == 0.0f) ? &SetPixelColor : &MixPixelColor;
			Point pt;
			Point p1;
			float u, v;
			float e1, e2, e3;
			float depthl;
#endif // !AVX

			// divide vertex-attribute by the vertex z-coordinate
			c0.R /= v0.z, c0.G /= v0.z, c0.B /= v0.z;
			c1.R /= v1.z, c1.G /= v1.z, c1.B /= v1.z;
			c2.R /= v2.z, c2.G /= v2.z, c2.B /= v2.z;

			//Vector2 uv0(0,0), uv1(1,0), uv2(0,1);
			uv0.x /= v0.z, uv0.y /= v0.z;
			uv1.x /= v1.z, uv1.y /= v1.z;
			uv2.x /= v2.z, uv2.y /= v2.z;

			// pre-compute 1 over z
			v0.z = 1 / v0.z, v1.z = 1 / v1.z, v2.z = 1 / v2.z;

			int cn = texture->channels();
			bool mode = !texture->empty();


#ifdef AVX


				//__m256* AVXset2 = new __m256[20];
				//__m256* subResult = new __m256[20];
				const __m256 subResult_1 = _mm256_set1_ps(v1.y - v0.y);
				const __m256 subResult_3 = _mm256_set1_ps(v1.x - v0.x);
				const __m256 subResult_5 = _mm256_set1_ps(v2.y - v1.y);
				const __m256 subResult_7 = _mm256_set1_ps(v2.x - v1.x);
				const __m256 subResult_9 = _mm256_set1_ps(v0.y - v2.y);
				const __m256 subResult_11 = _mm256_set1_ps(v0.x - v2.x);

				//----------
				const __m256 AVXset2_0 = _mm256_set1_ps(v0.x);
				const __m256 AVXset2_2 = _mm256_set1_ps(v0.y);
				const __m256 AVXset2_4 = _mm256_set1_ps(v1.x);
				const __m256 AVXset2_6 = _mm256_set1_ps(v1.y);
				const __m256 AVXset2_8 = _mm256_set1_ps(v2.x);
				const __m256 AVXset2_10 = _mm256_set1_ps(v2.y);
			

			__m256* ptx = 0;
			__m256* pty = 0;
			const __m256 textureRows = _mm256_set1_ps(texture->rows);
			const __m256 textureCols = _mm256_set1_ps(texture->cols - 1);

			__m256 v0z = _mm256_set1_ps(v0.z);
			__m256 v1z = _mm256_set1_ps(v1.z);
			__m256 v2z = _mm256_set1_ps(v2.z);

			const __m256 areaSet = _mm256_set1_ps(area);


			const __m256 c0R = _mm256_set1_ps(c0.R);
			const __m256 c0G = _mm256_set1_ps(c0.G);
			const __m256 c0B = _mm256_set1_ps(c0.B);
			const __m256 c1R = _mm256_set1_ps(c1.R);
			const __m256 c1G = _mm256_set1_ps(c1.G);
			const __m256 c1B = _mm256_set1_ps(c1.B);
			const __m256 c2R = _mm256_set1_ps(c2.R);
			const __m256 c2G = _mm256_set1_ps(c2.G);
			const __m256 c2B = _mm256_set1_ps(c2.B);

			const __m256 uv0x = _mm256_set1_ps(uv0.x);
			const __m256 uv1x = _mm256_set1_ps(uv1.x);
			const __m256 uv2x = _mm256_set1_ps(uv2.x);
			const __m256 uv0y = _mm256_set1_ps(uv0.y);
			const __m256 uv1y = _mm256_set1_ps(uv1.y);
			const __m256 uv2y = _mm256_set1_ps(uv2.y);
			float* cxR = 0;
			float* cxG = 0;
			float* cxB = 0;
			float* texturePixelId = 0;
			float* pixelId;
			__m256 cn_256 = _mm256_set1_ps(cn);

			__m256 avxset0, avxset1;

			void (*AVXpixel_placer)(int, ColorA*) = (transparency == 0.0f) ? &idSetPixelColor : &idMixPixelColor;
			
#endif // AVX

#if defined AVX && defined SMART_AVX
			if (!avxMode)
#endif
#if !defined AVX || defined SMART_AVX
			{
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
							depthl = 1 / (e1 * v0.z + e2 * v1.z + e3 * v2.z);
							if (depth[x][y] > depthl)
							{
								p1.x = x;
								p1.y = y;

								if (mode)
								{
									u = (e1 * uv0.x + e2 * uv1.x + e3 * uv2.x) * depthl;
									v = (e1 * uv0.y + e2 * uv1.y + e3 * uv2.y) * depthl;
									pt.x = u * texture->rows;
									pt.y = v * (texture->cols - 1);
									GetPixelColor(&pt, &cx, cn, texture);
								}
								else
								{
									cx.R = (uchar)((e1 * c0.R + e2 * c1.R + e3 * c2.R) * depthl);
									cx.G = (uchar)((e1 * c0.G + e2 * c1.G + e3 * c2.G) * depthl);
									cx.B = (uchar)((e1 * c0.B + e2 * c1.B + e3 * c2.B) * depthl);
								}


								(*pixel_placer)(&p1, &cx);
								depth[x][y] = depthl;
							}
							end = true;
						}
						else if (end)
						{
							break;
						}
					}
				}
			}
#endif // nAVX

#if defined AVX && defined SMART_AVX
			else
#endif
#ifdef AVX
			{
				//-------------------------------------------------
				for (int x = bbmin.x; x <= bbmax.x; x++)
				{
					avxset0 = _mm256_set1_ps(x);

					end = false;
					for (int y = bbmin.y; y <= bbmax.y; )
					{

						avxset1 = _mm256_add_ps(_mm256_set1_ps(y), avx::a2);

						const __m256 e3_256 = _mm256_div_ps(_mm256_fmsub_ps(_mm256_sub_ps(avxset0, AVXset2_0), subResult_1, _mm256_mul_ps(_mm256_sub_ps(avxset1, AVXset2_2), subResult_3)), areaSet);
						const __m256 e1_256 = _mm256_div_ps(_mm256_fmsub_ps(_mm256_sub_ps(avxset0, AVXset2_4), subResult_5, _mm256_mul_ps(_mm256_sub_ps(avxset1, AVXset2_6), subResult_7)), areaSet);
						const __m256 e2_256 = _mm256_div_ps(_mm256_fmsub_ps(_mm256_sub_ps(avxset0, AVXset2_8), subResult_9, _mm256_mul_ps(_mm256_sub_ps(avxset1, AVXset2_10), subResult_11)), areaSet);

						const int insideI = _mm256_movemask_ps(_mm256_and_ps(_mm256_cmp_ps(e1_256, _mm256_set1_ps(0), _CMP_GE_OQ), _mm256_and_ps(_mm256_cmp_ps(e2_256, _mm256_set1_ps(0), _CMP_GE_OQ), _mm256_cmp_ps(e3_256, _mm256_set1_ps(0), _CMP_GE_OQ))));
						if (insideI != 0)
						{

							const __m256 depthl_256 = _mm256_div_ps(avx::oneSet, avx::AVXinterpolate(e1_256, v0z, e2_256, v1z, e3_256, v2z));
							pixelId = avx::AVXpoint2PixelId(avxset0, avxset1, avx::displayChannels, avx::displayRows);

							if (mode)
							{
								ptx = &_mm256_mul_ps(_mm256_mul_ps(avx::AVXinterpolate(e1_256, uv0x, e2_256, uv1x, e3_256, uv2x), depthl_256), textureRows);
								pty = &_mm256_mul_ps(_mm256_mul_ps(avx::AVXinterpolate(e1_256, uv0y, e2_256, uv1y, e3_256, uv2y), depthl_256), textureCols);
								texturePixelId = avx::AVXpoint2PixelId(*ptx, *pty, cn_256, textureRows);
							}
							else
							{
								cxR = (float*)&_mm256_mul_ps(avx::AVXinterpolate(e1_256, c0R, e2_256, c1R, e3_256, c2R), depthl_256);
								cxG = (float*)&_mm256_mul_ps(avx::AVXinterpolate(e1_256, c0G, e2_256, c1G, e3_256, c2G), depthl_256);
								cxB = (float*)&_mm256_mul_ps(avx::AVXinterpolate(e1_256, c0B, e2_256, c1B, e3_256, c2B), depthl_256);
							}

							int i;
							for (i = 0; i < 8; i++)
							{
								if (((insideI >> i) & 1) == 1)
								{
									if (depth[x][y] > ((float*)&depthl_256)[i])
									{
										if (mode)
										{
											idGetPixelColor(texturePixelId[i], &cx, texture);
										}
										else
										{
											cx.R = (uchar)cxR[i];
											cx.G = (uchar)cxG[i];
											cx.B = (uchar)cxB[i];
										}

										(*AVXpixel_placer)(pixelId[i], &cx);
										depth[x][y] = ((float*)&depthl_256)[i];
									}
									end = true;
								}
								else if (end)
								{
									break;
								}

								y++;
								if (y >= height) break;
							}
							if (i < 8) break;
						}
						else
						{
							y += 8;
						}
					}
				}
			}
#endif // AVX

		}
	}
};

