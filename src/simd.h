/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "globals.h"

#if defined(__AVX512F__) && defined(__AVX512BW__)

using Vector = __m512i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t); // 32

inline Vector simd_zero() {
    return _mm512_setzero_si512();
}

inline Vector simd_load(const Vector *ptr) {
    return _mm512_load_si512(ptr);
}

inline Vector simd_set1_epi16(int16_t v) {
    return _mm512_set1_epi16(v);
}

inline Vector simd_max_epi16(Vector a, Vector b) {
    return _mm512_max_epi16(a, b);
}

inline Vector simd_min_epi16(Vector a, Vector b) {
    return _mm512_min_epi16(a, b);
}

inline Vector simd_mullo_epi16(Vector a, Vector b) {
    return _mm512_mullo_epi16(a, b);
}

inline Vector simd_madd_epi16(Vector a, Vector b) {
    return _mm512_madd_epi16(a, b);
}

inline Vector simd_add_epi32(Vector a, Vector b) {
    return _mm512_add_epi32(a, b);
}

inline int simd_reduce_add_epi32(Vector v) {
    return _mm512_reduce_add_epi32(v);
}

#elif defined(__AVX2__)

using Vector = __m256i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t); // 16

inline Vector simd_zero() {
    return _mm256_setzero_si256();
}

inline Vector simd_load(const Vector *ptr) {
    return _mm256_load_si256(ptr);
}

inline Vector simd_set1_epi16(int16_t v) {
    return _mm256_set1_epi16(v);
}

inline Vector simd_max_epi16(Vector a, Vector b) {
    return _mm256_max_epi16(a, b);
}

inline Vector simd_min_epi16(Vector a, Vector b) {
    return _mm256_min_epi16(a, b);
}

inline Vector simd_mullo_epi16(Vector a, Vector b) {
    return _mm256_mullo_epi16(a, b);
}

inline Vector simd_madd_epi16(Vector a, Vector b) {
    return _mm256_madd_epi16(a, b);
}

inline Vector simd_add_epi32(Vector a, Vector b) {
    return _mm256_add_epi32(a, b);
}

inline int simd_reduce_add_epi32(Vector v) {
    // Fold 256-bit → 128-bit → 64-bit → 32-bit
    __m128i lo  = _mm256_castsi256_si128(v);
    __m128i hi  = _mm256_extracti128_si256(v, 1);
    __m128i s   = _mm_add_epi32(lo, hi);
    __m128i hi64 = _mm_unpackhi_epi64(s, s);
    s = _mm_add_epi32(s, hi64);
    __m128i hi32 = _mm_shuffle_epi32(s, _MM_SHUFFLE(2, 3, 0, 1));
    s = _mm_add_epi32(s, hi32);
    return _mm_cvtsi128_si32(s);
}

#else

using Vector = __m128i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t); // 8

inline Vector simd_zero() {
    return _mm_setzero_si128();
}

inline Vector simd_load(const Vector *ptr) {
    return _mm_load_si128(ptr);
}

inline Vector simd_set1_epi16(int16_t v) {
    return _mm_set1_epi16(v);
}

inline Vector simd_max_epi16(Vector a, Vector b) {
    return _mm_max_epi16(a, b);
}

inline Vector simd_min_epi16(Vector a, Vector b) {
    return _mm_min_epi16(a, b);
}

inline Vector simd_mullo_epi16(Vector a, Vector b) {
    return _mm_mullo_epi16(a, b);
}

inline Vector simd_madd_epi16(Vector a, Vector b) {
    return _mm_madd_epi16(a, b);
}

inline Vector simd_add_epi32(Vector a, Vector b) {
    return _mm_add_epi32(a, b);
}

inline int simd_reduce_add_epi32(Vector v) {
    __m128i hi64 = _mm_unpackhi_epi64(v, v);
    __m128i s    = _mm_add_epi32(v, hi64);
    __m128i hi32 = _mm_shuffle_epi32(s, _MM_SHUFFLE(2, 3, 0, 1));
    s = _mm_add_epi32(s, hi32);
    return _mm_cvtsi128_si32(s);
}

#endif