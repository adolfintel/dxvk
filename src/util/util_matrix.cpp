#include "util_matrix.h"

namespace dxvk {

        Vector4& Matrix4::operator[](size_t index)       { return data[index]; }
  const Vector4& Matrix4::operator[](size_t index) const { return data[index]; }

  bool Matrix4::operator==(const Matrix4& m2) const {
    const Matrix4& m1 = *this;
    for (uint32_t i = 0; i < 4; i++) {
      if (m1[i] != m2[i])
        return false;
    }
    return true;
  }

  bool Matrix4::operator!=(const Matrix4& m2) const { return !operator==(m2); }

  Matrix4 Matrix4::operator+(const Matrix4& other) const {
    Matrix4 mat;
    for (uint32_t i = 0; i < 4; i++)
      mat[i] = data[i] + other.data[i];
    return mat;
  }

  Matrix4 Matrix4::operator-(const Matrix4& other) const {
    Matrix4 mat;
    for (uint32_t i = 0; i < 4; i++)
      mat[i] = data[i] - other.data[i];
    return mat;
  }

  Matrix4 Matrix4::operator*(const Matrix4& m2) const {
    Matrix4 result;
    for (uint32_t i = 0; i < 4; i++)
      result[i] = (*this) * m2[i];
    return result;
  }

  Vector4 Matrix4::operator*(const Vector4& v) const {
    Vector4 result;
    __m128 vec  = _mm_load_ps(&v.x);
    __m128 mul0 = _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0x00), _mm_load_ps(&data[0].x));
    __m128 mul1 = _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0x55), _mm_load_ps(&data[1].x));
    __m128 mul2 = _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0xAA), _mm_load_ps(&data[2].x));
    __m128 mul3 = _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0xFF), _mm_load_ps(&data[3].x));

    __m128 add0 = _mm_add_ps(mul0, mul1);
    __m128 add1 = _mm_add_ps(mul2, mul3);

    _mm_store_ps(&result.x, _mm_add_ps(add0, add1));
    return result;
  }

  Matrix4 Matrix4::operator*(float scalar) const {
    Matrix4 mat;
    for (uint32_t i = 0; i < 4; i++)
      mat[i] = data[i] * scalar;
    return mat;
  }

  Matrix4 Matrix4::operator/(float scalar) const {
    Matrix4 mat;
    for (uint32_t i = 0; i < 4; i++)
      mat[i] = data[i] / scalar;
    return mat;
  }

  Matrix4& Matrix4::operator+=(const Matrix4& other) {
    for (uint32_t i = 0; i < 4; i++)
      data[i] += other.data[i];
    return *this;
  }

  Matrix4& Matrix4::operator-=(const Matrix4& other) {
    for (uint32_t i = 0; i < 4; i++)
      data[i] -= other.data[i];
    return *this;
  }

  Matrix4& Matrix4::operator*=(const Matrix4& other) {
    return (*this = (*this) * other);
  }

  Matrix4 transpose(const Matrix4& m) {
    Matrix4 result;
    __m128 row0 = _mm_load_ps(&m.data[0].x);
    __m128 row1 = _mm_load_ps(&m.data[1].x);
    __m128 row2 = _mm_load_ps(&m.data[2].x);
    __m128 row3 = _mm_load_ps(&m.data[3].x);

    __m128 tmp0 = _mm_shuffle_ps(row0, row1, 0x44);
    __m128 tmp1 = _mm_shuffle_ps(row2, row3, 0x44);
    __m128 tmp2 = _mm_shuffle_ps(row0, row1, 0xee);
    __m128 tmp3 = _mm_shuffle_ps(row2, row3, 0xee);

    _mm_store_ps(&result.data[0].x, _mm_shuffle_ps(tmp0, tmp1, 0x88));
    _mm_store_ps(&result.data[0].y, _mm_shuffle_ps(tmp0, tmp1, 0xdd));
    _mm_store_ps(&result.data[0].z, _mm_shuffle_ps(tmp2, tmp3, 0x88));
    _mm_store_ps(&result.data[0].w, _mm_shuffle_ps(tmp2, tmp3, 0xdd));
    return result;
  }

  Matrix4 inverse(const Matrix4& m)
  {
    float coef00    = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    float coef02    = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float coef03    = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    float coef04    = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    float coef06    = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    float coef07    = m[1][1] * m[2][3] - m[2][1] * m[1][3];
    float coef08    = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    float coef10    = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    float coef11    = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    float coef12    = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    float coef14    = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    float coef15    = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    float coef16    = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    float coef18    = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    float coef19    = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    float coef20    = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float coef22    = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float coef23    = m[1][0] * m[2][1] - m[2][0] * m[1][1];
  
    Vector4 fac0    = { coef00, coef00, coef02, coef03 };
    Vector4 fac1    = { coef04, coef04, coef06, coef07 };
    Vector4 fac2    = { coef08, coef08, coef10, coef11 };
    Vector4 fac3    = { coef12, coef12, coef14, coef15 };
    Vector4 fac4    = { coef16, coef16, coef18, coef19 };
    Vector4 fac5    = { coef20, coef20, coef22, coef23 };
  
    Vector4 vec0    = { m[1][0], m[0][0], m[0][0], m[0][0] };
    Vector4 vec1    = { m[1][1], m[0][1], m[0][1], m[0][1] };
    Vector4 vec2    = { m[1][2], m[0][2], m[0][2], m[0][2] };
    Vector4 vec3    = { m[1][3], m[0][3], m[0][3], m[0][3] };
  
    Vector4 inv0    = { vec1 * fac0 - vec2 * fac1 + vec3 * fac2 };
    Vector4 inv1    = { vec0 * fac0 - vec2 * fac3 + vec3 * fac4 };
    Vector4 inv2    = { vec0 * fac1 - vec1 * fac3 + vec3 * fac5 };
    Vector4 inv3    = { vec0 * fac2 - vec1 * fac4 + vec2 * fac5 };
  
    Vector4 signA   = { +1, -1, +1, -1 };
    Vector4 signB   = { -1, +1, -1, +1 };
    Matrix4 inverse = { inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB };

    Vector4 row0    = { inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0] };

    Vector4 dot0    = { m[0] * row0 };
    float dot1      = (dot0.x + dot0.y) + (dot0.z + dot0.w);

    return inverse * (1.0f / dot1);
  }

  std::ostream& operator<<(std::ostream& os, const Matrix4& m) {
    os << "Matrix4(";
    for (uint32_t i = 0; i < 4; i++) {
      os << "\n\t" << m[i];
      if (i < 3)
        os << ", ";
    }
    os << "\n)";

    return os;
  }

}