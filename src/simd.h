#pragma once

#include "globals.h"
#include "immintrin.h"

#if defined(__AVX512F__) && defined(__AVX512BW__)
    using Vector = __m512i;

    inline Vector clip(Vector vector, int max) {
        return _mm512_max_epi16(_mm512_min_epi16(vector, _mm512_set1_epi16(max)), _mm512_setzero_si512());
    }

    inline Vector loadToVector(std::span<int16_t, layer1Size> span, int index) {
        return _mm512_load_si512(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector loadWeightsToVector(std::array<int16_t, layer1Size * 2> span, int index) {
        return _mm512_load_si512(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector zeroVector() {
        return _mm512_setzero_si512();
    }

    inline Vector mullo(Vector x, Vector y) {
        return _mm512_mullo_epi16(x, y);
    }

    inline Vector add(Vector x, Vector y) {
        return _mm512_add_epi32(x, y);
    }

    inline Vector madd(Vector x, Vector y) {
        return _mm512_madd_epi16(x, y);
    }

    inline int vectorSum(Vector vector) {
        return _mm512_reduce_add_epi32(vector);
    }
#elif defined(__AVX2__)
    using Vector = __m256i;

    inline Vector clip(Vector vector, int max) {
        return _mm256_max_epi16(_mm256_min_epi16(vector, _mm256_set1_epi16(max)), _mm256_setzero_si256());
    }

    inline Vector loadToVector(std::span<int16_t, layer1Size> span, int index) {
        return _mm256_load_si256(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector loadWeightsToVector(std::array<int16_t, layer1Size * 2> span, int index) {
        return _mm256_load_si256(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector zeroVector() {
        return _mm256_setzero_si256();
    }

    inline Vector mullo(Vector x, Vector y) {
        return _mm256_mullo_epi16(x, y);
    }

    inline Vector add(Vector x, Vector y) {
        return _mm256_add_epi32(x, y);
    }

    inline Vector madd(Vector x, Vector y) {
        return _mm256_madd_epi16(x, y);
    }

    inline int vectorSum(Vector vector) {
        __m128i sum0;
        __m128i sum1;
        // divide into halves
        sum0 = _mm256_castsi256_si128(vector);
        sum1 = _mm256_extracti128_si256(vector, 1);
        // add the halves
        sum0 = _mm_add_epi32(sum0, sum1);
        // get half of the result
        sum1 = _mm_unpackhi_epi64(sum0, sum0);
        // add the halves:
        sum0 = _mm_add_epi32(sum0, sum1);
        // reorder so it's right
        sum1 = _mm_shuffle_epi32(sum0, _MM_SHUFFLE(2, 3, 0, 1));
        // final add
        sum0 = _mm_add_epi32(sum0, sum1);
        // cast back to int
        return _mm_cvtsi128_si32(sum0);
    }
#else
    using Vector = __m128i;

    inline Vector clip(Vector vector, int max) {
        return _mm_max_epi16(_mm_min_epi16(vector, _mm_set1_epi16(max)), _mm_setzero_si128());
    }

    inline Vector loadToVector(std::span<int16_t, layer1Size> span, int index) {
        return _mm_load_si(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector loadWeightsToVector(std::array<int16_t, layer1Size * 2> span, int index) {
        return _mm_load_si(reinterpret_cast<const Vector *>(&span[index]));
    }

    inline Vector zeroVector() {
        return _mm_setzero_si128();
    }

    inline Vector mullo(Vector x, Vector y) {
        return _mm_mullo_epi16(x, y);
    }

    inline Vector add(Vector x, Vector y) {
        return _mm_add_epi32(x, y);
    }

    inline Vector madd(Vector x, Vector y) {
        return _mm_madd_epi16(x, y);
    }

    // "the normal way:tm:" ~ Ciekce, 2024
    inline int vectorSum(Vector vector) {
        const auto high64 = _mm_unpackhi_epi64(v, v);
        const auto sum64 = _mm_add_epi32(v, high64);

        const auto high32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
        const auto sum32 = _mm_add_epi32(sum64, high32);

        return _mm_cvtsi128_si32(sum32);
    }

#endif