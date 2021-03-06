#include "matrix3x3.h"

namespace KtStd::Math
{
	void mmul_tiled_avx_unrolled(const int n, const float* left, const float* right, float* result) {
		const int block_width = n >= 256 ? 512 : 256;
		const int block_height = n >= 512 ? 8 : n >= 256 ? 16 : 32;
		for (int column_offset = 0; column_offset < n; column_offset += block_width) {
			for (int row_offset = 0; row_offset < n; row_offset += block_height) {
				for (int i = 0; i < n; ++i) {
					for (int j = column_offset; j < column_offset + block_width && j < n; j += 64) {
						__m256 sum1 = _mm256_load_ps(result + i * n + j);
						__m256 sum2 = _mm256_load_ps(result + i * n + j + 8);
						__m256 sum3 = _mm256_load_ps(result + i * n + j + 16);
						__m256 sum4 = _mm256_load_ps(result + i * n + j + 24);
						__m256 sum5 = _mm256_load_ps(result + i * n + j + 32);
						__m256 sum6 = _mm256_load_ps(result + i * n + j + 40);
						__m256 sum7 = _mm256_load_ps(result + i * n + j + 48);
						__m256 sum8 = _mm256_load_ps(result + i * n + j + 56);
						for (int k = row_offset; k < row_offset + block_height && k < n; ++k) {
							__m256 multiplier = _mm256_set1_ps(left[i * n + k]);
							sum1 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j), sum1);
							sum2 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 8), sum2);
							sum3 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 16), sum3);
							sum4 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 24), sum4);
							sum5 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 32), sum5);
							sum6 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 40), sum6);
							sum7 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 48), sum7);
							sum8 = _mm256_fmadd_ps(multiplier, _mm256_load_ps(right + k * n + j + 56), sum8);
						}
						_mm256_store_ps(result + i * n + j, sum1);
						_mm256_store_ps(result + i * n + j + 8, sum2);
						_mm256_store_ps(result + i * n + j + 16, sum3);
						_mm256_store_ps(result + i * n + j + 24, sum4);
						_mm256_store_ps(result + i * n + j + 32, sum5);
						_mm256_store_ps(result + i * n + j + 40, sum6);
						_mm256_store_ps(result + i * n + j + 48, sum7);
						_mm256_store_ps(result + i * n + j + 56, sum8);
					}
				}
			}
		}
	}



	matrix3x3& MatAddAlg(const matrix3x3& lhs, const matrix3x3& rhs)
	{
		matrix3x3 res;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				res.mat[i][j] = lhs.mat[i][j] + rhs.mat[i][j];
			}
		}
		return res;
	}

	matrix3x3& MatSubAlg(const matrix3x3& lhs, const matrix3x3& rhs)
	{
		matrix3x3 res;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				res.mat[i][j] = lhs.mat[i][j] + rhs.mat[i][j];
			}
		}
		return res;
	}

	matrix3x3& MatMulAlg(const matrix3x3& lhs, const matrix3x3& rhs)
	{
		matrix3x3 temp;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					temp.mat[i][j] += lhs.mat[i][k] * rhs.mat[k][j];
				}
			}
		}
		return temp;
	}


	void matrix3x3::MatMul(const matrix3x3& rhs)
	{
		*this = MatMulAlg(*this, rhs);
	}


	void matrix3x3::MatAdd(const matrix3x3& rhs)
	{
		*this = MatAddAlg(*this, rhs);
	}

	void matrix3x3::MatSub(const matrix3x3& rhs)
	{
		*this = MatSubAlg(*this, rhs);
	}

	void matrix3x3::ScalarMul(FloatFX scalar)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				this->mat[i][j] *= scalar;
			}
		}
	}

	void matrix3x3::ScalarAdd(FloatFX scalar)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				this->mat[i][j] += scalar;
			}
		}
	}

	void matrix3x3::operator+=(FloatFX rhs)
	{
		ScalarAdd(rhs);
	}

	void matrix3x3::operator*=(FloatFX rhs)
	{
		ScalarMul(rhs);
	}

	inline FloatFX matrix3x3::Det() const
	{
		return mat[0][0] * ((mat[1][1] * mat[2][2]) - (mat[2][1] * mat[1][2])) - mat[0][1] * (mat[1][0] * mat[2][2] - mat[2][0] * mat[1][2]) + mat[0][2] * (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]);
	}

	matrix3x3& matrix3x3::Inverse() const
	{
		double invdet = 1 / Det();

		matrix3x3 res;
		res.mat[0][0] = (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) * invdet;
		res.mat[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) * invdet;
		res.mat[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * invdet;
		res.mat[1][0] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) * invdet;
		res.mat[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * invdet;
		res.mat[1][2] = (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]) * invdet;
		res.mat[2][0] = (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) * invdet;
		res.mat[2][1] = (mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1]) * invdet;
		res.mat[2][2] = (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) * invdet;

		return res;
	}

	void matrix3x3::operator*=(const matrix3x3& rhs)
	{
		MatMul(rhs);
	}


	void matrix3x3::operator+=(const matrix3x3& rhs)
	{
		MatAdd(rhs);
	}


	void matrix3x3::operator-=(const matrix3x3& rhs)
	{
		MatSub(rhs);
	}

	matrix3x3& matrix3x3::operator+(const matrix3x3& rhs)
	{
		return MatAddAlg(*this, rhs);
	}

	matrix3x3& matrix3x3::operator-(const matrix3x3& rhs)
	{
		return MatSubAlg(*this, rhs);
	}

	matrix3x3& matrix3x3::operator*(const matrix3x3& rhs)
	{
		return MatMulAlg(*this, rhs);
	}


	void M3x3_SSE(float* A, float* B, float* C) {
		__m128 row1 = _mm_load_ps(&B[0]);
		__m128 row2 = _mm_load_ps(&B[3]);
		__m128 row3 = _mm_load_ps(&B[6]);
		for (int i = 0; i < 3; i++) {
			__m128 brod1 = _mm_set1_ps(A[3 * i + 0]);
			__m128 brod2 = _mm_set1_ps(A[3 * i + 1]);
			__m128 brod3 = _mm_set1_ps(A[3 * i + 2]);
			__m128 row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2)), _mm_mul_ps(brod3, row3));
			_mm_store_ps(&C[3 * i], row);
		}
	}
}