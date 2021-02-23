#pragma once

#include "Helper.h"
#include "AVX.h"

class Screen
{

public:
	static void CreateWindow()
	{
#ifdef OPENCV
		namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
		image = Mat::zeros(height, width, CV_8UC3);
#endif // OPENCV

#ifdef SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		window = SDL_CreateWindow
		(
			"CharEngine",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height,
			SDL_WINDOW_SHOWN
		);

		renderer = SDL_CreateRenderer
		(
			window,
			-1,
			SDL_RENDERER_ACCELERATED
		);


		texture = SDL_CreateTexture
		(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			width, height
		);
		image.initialize(height, width, 4);
#endif // SDL		

	}

	static Point* PosToScreenCenter(Point* pos)
	{
		pos->x += halfOfWidth;
		pos->y = halfOfHeight - pos->y;
		return pos;
	}

	static Vector3* PosToScreenCenter(Vector3* pos)
	{
		pos->x += halfOfWidth;
		pos->y = halfOfHeight - pos->y;
		return pos;
	}

	static void SetPixelColor(Point* point, ColorA* color)
	{
		int i = point->y * width * channels + point->x * channels;
		image.data[i + R_OFFSET] = color->R;
		image.data[i + G_OFFSET] = color->G;
		image.data[i + B_OFFSET] = color->B;
	}

	static void MixPixelColor(Point* point, ColorA* color)
	{
		int i = point->y * width * channels + point->x * channels;
		image.data[i + R_OFFSET] = (uchar)(((image.data[i + R_OFFSET]) - color->R) * color->A + color->R);
		image.data[i + G_OFFSET] = (uchar)(((image.data[i + G_OFFSET]) - color->G) * color->A + color->G);
		image.data[i + B_OFFSET] = (uchar)(((image.data[i + B_OFFSET]) - color->B) * color->A + color->B);
	}

	static void idSetPixelColor(int id, ColorA* color)
	{
		image.data[id + R_OFFSET] = color->R;
		image.data[id + G_OFFSET] = color->G;
		image.data[id + B_OFFSET] = color->B;
	}

	static void idMixPixelColor(int id, ColorA* color)
	{
		image.data[id + R_OFFSET] = (uchar)(((image.data[id + R_OFFSET]) - color->R) * color->A + color->R);
		image.data[id + G_OFFSET] = (uchar)(((image.data[id + G_OFFSET]) - color->G) * color->A + color->G);
		image.data[id + B_OFFSET] = (uchar)(((image.data[id + B_OFFSET]) - color->B) * color->A + color->B);
	}

	static void GetPixelColor(Point* point, ColorA* color, int cn, Mat* im)
	{
		int i = point->y * im->rows * cn + point->x * cn;
		color->R = im->data[i];
		color->G = im->data[i + 1];
		color->B = im->data[i + 2];
	}

	static void idGetPixelColor(int id, ColorA* color, Mat* im)
	{
		color->R = im->data[id];
		color->G = im->data[id + 1];
		color->B = im->data[id + 2];
	}

	static void PrintFrame()
	{
#ifdef OPENCV
		imshow("Display window", image);
		image.setTo(Scalar(140, 80, 10));
#endif // OPENCV
#ifdef SDL
		SDL_UpdateTexture
		(
			texture,
			NULL,
			image.data,
			width * 4
		);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		//clear image
		memset(image.data, 100, image.dataSize);
#endif // SDL
		fill_n(&depth[0][0], width * height, 255);

	}
#ifdef OPENCV
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
#endif // OPENCV

	static void DrawTriangle(Triangle& triangle, Tile* tile, Mat* texture)
	{
		bool useDepth = triangle.type != ActorType::Skybox;

		fColor c0(triangle.materialp->color);
		fColor c1(triangle.materialp->color);
		fColor c2(triangle.materialp->color);


		Point bbmin(max(triangle.bbmin.x, tile->pmin.x), max(triangle.bbmin.y, tile->pmin.y));
		Point bbmax(min(triangle.bbmax.x, tile->pmax.x), min(triangle.bbmax.y, tile->pmax.y));

		//face specularity effect
		float specularity = (1.0f + triangle.normal.z) * 120.0f + 0.8f;
		specularity = (specularity > 1.0f) ? 1.0f : ((specularity < 0.0f) ? 0.0f : specularity);

		bool inside = true;
		bool end = false;

		const float area = (triangle.v2.x - triangle.v0.x) * (triangle.v1.y - triangle.v0.y) - (triangle.v2.y - triangle.v0.y) * (triangle.v1.x - triangle.v0.x);

		ColorA cx(0, 0, 0, triangle.materialp->transparency);
		c0 *= specularity;
		c1 *= specularity;
		c2 *= specularity;

#ifndef AVX
		void (*pixel_placer)(Point*, ColorA*) = (triangle.materialp->transparency == 0.0f) ? &SetPixelColor : &MixPixelColor;
		Point pt;
		Point p1;
		float u, v;
		float e1, e2, e3;
		float depthl;
#endif // not AVX

		// divide vertex-attribute by the vertex z-coordinate
		c0.R /= triangle.v0.z, c0.G /= triangle.v0.z, c0.B /= triangle.v0.z;
		c1.R /= triangle.v1.z, c1.G /= triangle.v1.z, c1.B /= triangle.v1.z;
		c2.R /= triangle.v2.z, c2.G /= triangle.v2.z, c2.B /= triangle.v2.z;

		//Vector2 uv0(0,0), uv1(1,0), uv2(0,1);
		triangle.uv0.x /= triangle.v0.z, triangle.uv0.y /= triangle.v0.z;
		triangle.uv1.x /= triangle.v1.z, triangle.uv1.y /= triangle.v1.z;
		triangle.uv2.x /= triangle.v2.z, triangle.uv2.y /= triangle.v2.z;

		// pre-compute 1 over z
		triangle.v0.z = 1 / triangle.v0.z, triangle.v1.z = 1 / triangle.v1.z, triangle.v2.z = 1 / triangle.v2.z;

		int cn = texture->channels();
		bool mode = !texture->empty();

#ifdef AVX

		//__m256* AVXset2 = new __m256[20];
		//__m256* subResult = new __m256[20];
		const __m256 subResult_1 = _mm256_set1_ps(triangle.v1.y - triangle.v0.y);
		const __m256 subResult_3 = _mm256_set1_ps(triangle.v1.x - triangle.v0.x);
		const __m256 subResult_5 = _mm256_set1_ps(triangle.v2.y - triangle.v1.y);
		const __m256 subResult_7 = _mm256_set1_ps(triangle.v2.x - triangle.v1.x);
		const __m256 subResult_9 = _mm256_set1_ps(triangle.v0.y - triangle.v2.y);
		const __m256 subResult_11 = _mm256_set1_ps(triangle.v0.x - triangle.v2.x);

		const __m256 AVXset2_0 = _mm256_set1_ps(triangle.v0.x);
		const __m256 AVXset2_2 = _mm256_set1_ps(triangle.v0.y);
		const __m256 AVXset2_4 = _mm256_set1_ps(triangle.v1.x);
		const __m256 AVXset2_6 = _mm256_set1_ps(triangle.v1.y);
		const __m256 AVXset2_8 = _mm256_set1_ps(triangle.v2.x);
		const __m256 AVXset2_10 = _mm256_set1_ps(triangle.v2.y);

		__m256* ptx = 0;
		__m256* pty = 0;
		const __m256 textureRows = _mm256_set1_ps(texture->rows);
		const __m256 textureCols = _mm256_set1_ps(texture->cols - 1);

		__m256 v0z = _mm256_set1_ps(triangle.v0.z);
		__m256 v1z = _mm256_set1_ps(triangle.v1.z);
		__m256 v2z = _mm256_set1_ps(triangle.v2.z);

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

		const __m256 uv0x = _mm256_set1_ps(triangle.uv0.x);
		const __m256 uv1x = _mm256_set1_ps(triangle.uv1.x);
		const __m256 uv2x = _mm256_set1_ps(triangle.uv2.x);
		const __m256 uv0y = _mm256_set1_ps(triangle.uv0.y);
		const __m256 uv1y = _mm256_set1_ps(triangle.uv1.y);
		const __m256 uv2y = _mm256_set1_ps(triangle.uv2.y);
		float* cxR = 0;
		float* cxG = 0;
		float* cxB = 0;
		float* texturePixelId = 0;
		float* pixelId;
		__m256 cn_256 = _mm256_set1_ps(cn);

		__m256 avxset0, avxset1;

		void (*AVXpixel_placer)(int, ColorA*) = (triangle.materialp->transparency == 0.0f) ? &idSetPixelColor : &idMixPixelColor;

#else

		for (int x = bbmin.x; x <= bbmax.x; x++)
		{
			end = false;
			for (int y = bbmin.y; y <= bbmax.y; y++)
			{
				inside = true;
				//checking if the pixel is inside the triangle (cross product)
				inside = (e3 = (x - triangle.v0.x) * (triangle.v1.y - triangle.v0.y) - (y - triangle.v0.y) * (triangle.v1.x - triangle.v0.x)) >= 0;
				if (!inside)
					continue;

				inside = (e1 = (x - triangle.v1.x) * (triangle.v2.y - triangle.v1.y) - (y - triangle.v1.y) * (triangle.v2.x - triangle.v1.x)) >= 0;
				if (!inside)
					continue;

				inside = (e2 = (x - triangle.v2.x) * (triangle.v0.y - triangle.v2.y) - (y - triangle.v2.y) * (triangle.v0.x - triangle.v2.x)) >= 0;

				if (inside)
				{
					e1 /= area;
					e2 /= area;
					e3 /= area;
					depthl = 1 / (e1 * triangle.v0.z + e2 * triangle.v1.z + e3 * triangle.v2.z);
					if (depth[x][y] > depthl)
					{
						p1.x = x;
						p1.y = y;

						if (mode)
						{
							u = (e1 * triangle.uv0.x + e2 * triangle.uv1.x + e3 * triangle.uv2.x) * depthl;
							v = (e1 * triangle.uv0.y + e2 * triangle.uv1.y + e3 * triangle.uv2.y) * depthl;
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
#endif // AVX

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
								if (useDepth)
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
								}
								else
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
};

