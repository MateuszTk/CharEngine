#pragma once

// check if AVX is supported
#if !defined(DISABLE_AVX) && defined(__AVX__) && defined(__AVX2__) 

// referenced in other files
#define AVX

#include <immintrin.h>
#include "Helper.hpp"
namespace CharEngine {
	namespace avx {
		const __m256 a2 = _mm256_setr_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
		const __m256i a2i = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

		const __m256 oneSet = _mm256_set1_ps(1);
		const __m256i displayChannels = _mm256_set1_epi32(Config::channels);
		__m256i displayRows = _mm256_set1_epi32(Config::width);


		inline void updateSet(float* set, float value) {
			int i = 8;
			while (i-- > 0) {
				*(set++) = value;
			}
			set -= 8;
		}

		inline void updateTripleSet(float* set0, float* set1, float* set2, float value) {
			int i = 8;
			while (i-- > 0) {
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

		inline __m256i AVXpoint2PixelId(__m256i x, __m256i y, __m256i rows) {
			return _mm256_add_epi32(_mm256_mullo_epi32(y, rows), x);
		}

		inline __m256 AVXpoint2TexelId(__m256 x, __m256 y, __m256 ch, __m256 rows) {
			return _mm256_mul_ps(_mm256_fmadd_ps(_mm256_floor_ps(y), rows, _mm256_floor_ps(x)), ch);
		}
	}
}
#endif // if AVX and AVX2
