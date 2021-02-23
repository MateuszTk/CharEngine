#pragma once

// check if AVX is supported
#if !defined(DISABLE_AVX) && defined(__AVX__) && defined(__AVX2__) 

// referenced in other files
#define AVX

#include <immintrin.h>
#include "Helper.h"

namespace avx
{
	inline float* mulitply8f(__m256 a, __m256 b)
	{
		return (float*)(&_mm256_mul_ps(a, b));
	}


	const __m256 a2 = _mm256_setr_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

	const __m256 oneSet = _mm256_set1_ps(1);
	const __m256 displayChannels = _mm256_set1_ps(channels);
	const __m256 displayRows = _mm256_set1_ps(width);


	inline void updateSet(float* set, float value)
	{
		int i = 8;
		while (i-- > 0)
		{
			*(set++) = value;
		}
		set -= 8;
	}

	inline void updateTripleSet(float* set0, float* set1, float* set2, float value)
	{
		int i = 8;
		while (i-- > 0)
		{
			*(set0++) = value;
			*(set1++) = value;
			*(set2++) = value;
		}
		set0 -= 8;
		set1 -= 8;
		set2 -= 8;
	}

	inline __m256 AVXinterpolate(__m256 e1, __m256 v1, __m256 e2, __m256 v2, __m256 e3, __m256 v3) //e1 * v0.z + e2 * v1.z + e3 * v2.z
	{
		return _mm256_fmadd_ps(e1, v1, _mm256_fmadd_ps(e2, v2, _mm256_mul_ps(e3, v3)));
	}

	inline float* AVXpoint2PixelId(__m256 x, __m256 y, __m256 cn, __m256 rows)
	{
		return (float*)&_mm256_mul_ps(_mm256_fmadd_ps(_mm256_floor_ps(y), rows, _mm256_floor_ps(x)), cn);
	}
}

#endif // if AVX and AVX2
