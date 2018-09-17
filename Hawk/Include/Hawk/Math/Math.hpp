#pragma once

#include <Hawk/Common/Defines.hpp>
#include <Hawk/Math/Detail/Vector.hpp>
#include <Hawk/Math/Detail/Matrix.hpp>
#include <Hawk/Math/Detail/Quaternion.hpp>

namespace Hawk {

	namespace Math {


		template<typename T> constexpr T PI = T(3.1415926535897932385L); 




		template<typename T>               using Quaternion = Detail::Quaternion<T>;
		template<typename T, U32 N>        using Vector     = Detail::Vector<T, N>;		
		template<typename T, U32 N, U32 M> using Matrix     = Detail::Matrix<T, N, M>;
		

		template<typename T> using Vec2_tpl = Vector<T, 2>;
		template<typename T> using Vec3_tpl = Vector<T, 3>;
		template<typename T> using Vec4_tpl = Vector<T, 4>;

		template<typename T> using Mat2x2_tpl = Matrix<T, 2, 2>;
		template<typename T> using Mat3x3_tpl = Matrix<T, 3, 3>;
		template<typename T> using Mat4x4_tpl = Matrix<T, 4, 4>;

		using Quat =  Quaternion<F32>;
		using Quatd = Quaternion<F64>;

		using Vec2 = Vec2_tpl<F32>;
		using Vec3 = Vec3_tpl<F32>;
		using Vec4 = Vec4_tpl<F32>;

		using Vec2d = Vec2_tpl<F64>;
		using Vec3d = Vec3_tpl<F64>;
		using Vec4d = Vec4_tpl<F64>;

		using Vec2i = Vec2_tpl<I32>;
		using Vec3i = Vec3_tpl<I32>;
		using Vec4i = Vec4_tpl<I32>;

		using Vec2u = Vec2_tpl<U32>;
		using Vec3u = Vec3_tpl<U32>;
		using Vec4u = Vec4_tpl<U32>;

		using Mat2x2 = Mat2x2_tpl<F32>;
		using Mat3x3 = Mat3x3_tpl<F32>;
		using Mat4x4 = Mat4x4_tpl<F32>;

		using Mat2x2d = Mat2x2_tpl<F64>;
		using Mat3x3d = Mat3x3_tpl<F64>;
		using Mat4x4d = Mat4x4_tpl<F64>;

		using Mat2x2i = Mat2x2_tpl<I32>;
		using Mat3x3i = Mat3x3_tpl<I32>;
		using Mat4x4i = Mat4x4_tpl<I32>;

		static_assert(sizeof(Quat)  == 4 * sizeof(F32), "Quat alignment error");
		static_assert(sizeof(Quatd) == 4 * sizeof(F64), "Quatd alignment error");


		static_assert(sizeof(Mat2x2) == 4 * sizeof(F32), "Mat2x2 alignment error");
		static_assert(sizeof(Mat3x3) == 9 * sizeof(F32), "Mat3x3 alignment error");
		static_assert(sizeof(Mat4x4) == 16 * sizeof(F32), "Mat4x4 alignment error");

		static_assert(sizeof(Mat2x2d) == 4 * sizeof(F64), "Mat2x2d alignment error");
		static_assert(sizeof(Mat3x3d) == 9 * sizeof(F64), "Mat3x3d alignment error");
		static_assert(sizeof(Mat4x4d) == 16 * sizeof(F64), "Mat4x4d alignment error");


		static_assert(sizeof(Mat2x2i) == 4 * sizeof(I32), "Mat2x2i alignment error");
		static_assert(sizeof(Mat3x3i) == 9 * sizeof(I32), "Mat3x3i alignment error");
		static_assert(sizeof(Mat4x4i) == 16 * sizeof(I32), "Mat4x4i alignment error");



		static_assert(sizeof(Vec2) == 2 * sizeof(F32), "Vec2 alignment error");
		static_assert(sizeof(Vec3) == 3 * sizeof(F32), "Vec3 alignment error");
		static_assert(sizeof(Vec4) == 4 * sizeof(F32), "Vec4 alignment error");


		static_assert(sizeof(Vec2d) == 2 * sizeof(F64), "Vec2d alignment error");
		static_assert(sizeof(Vec3d) == 3 * sizeof(F64), "Vec3d alignment error");
		static_assert(sizeof(Vec4d) == 4 * sizeof(F64), "Vec4d alignment error");


		static_assert(sizeof(Vec2i) == 2 * sizeof(I32), "Vec2i alignment error");
		static_assert(sizeof(Vec3i) == 3 * sizeof(I32), "Vec3i alignment error");
		static_assert(sizeof(Vec4i) == 4 * sizeof(I32), "Vec4i alignment error");


		template<typename T> constexpr auto Radians(T a) noexcept->T;
		template<typename T> constexpr auto Degrees(T a) noexcept->T;


		template<typename T> constexpr auto Abs(T a)  noexcept->T;
		template<typename T> constexpr auto Sin(T a)  noexcept->T;
		template<typename T> constexpr auto Cos(T a)  noexcept->T;
		template<typename T> constexpr auto Tan(T a)  noexcept->T;
		template<typename T> constexpr auto Pow(T a)  noexcept->T;
		template<typename T> constexpr auto Exp(T a)  noexcept->T;
		template<typename T> constexpr auto Log(T a)  noexcept->T;
		template<typename T> constexpr auto Asin(T a) noexcept->T;
		template<typename T> constexpr auto Acos(T a) noexcept->T;
		template<typename T> constexpr auto Atan(T a) noexcept->T;
		template<typename T> constexpr auto Sqrt(T a) noexcept->T;
		template<typename T> constexpr auto Sign(T a) noexcept->T;
		template<typename T> constexpr auto Lerp(T v0, T v1, T t) noexcept->T;


		template<typename T, U32 N> constexpr auto Abs(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Sin(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Cos(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Tan(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Pow(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Exp(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Log(Vector<T, N> const& v)  noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Asin(Vector<T, N> const& v) noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Acos(Vector<T, N> const& v) noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Atan(Vector<T, N> const& v) noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Sqrt(Vector<T, N> const& v) noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Sign(Vector<T, N> const& v) noexcept->Vector<T, N>;
		template<typename T, U32 N> constexpr auto Lerp(Vector<T, N> const& v0, Vector<T, N> const& v1, Vector<T, N> const& t) noexcept->Vector<T, N>;


		template<typename T, U32 N> constexpr auto Transpose(Matrix<T, N, N> const& m)                        noexcept->Matrix<T, N, N>;
		template<typename T, U32 N> constexpr auto Inverse(Matrix<T, N, N> const& m)                          noexcept->Matrix<T, N, N>;
		template<typename T, U32 N> constexpr auto Distance(Vector<T, N> const& lhs, Vector<T, N> const& rhs) noexcept->T;
		template<typename T, U32 N> constexpr auto Length(Vector<T, N> const& v)                              noexcept->T;
		template<typename T, U32 N> constexpr auto Dot(Vector<T, N> const& lhs, Vector<T, N> const& rhs)      noexcept->T;
		template<typename T, U32 N> constexpr auto Normalize(Vector<T, N> const& v)                           noexcept->Vector<T, N>;
		template<typename T>        constexpr auto Cross(Vec3_tpl<T> const& lhs, Vec3_tpl<T> const& rhs)      noexcept->Vec3_tpl<T>;

		template<typename T> constexpr auto Dot(Quaternion<T> const& lhs, Quaternion<T> const& rhs)           noexcept->T;
		template<typename T> constexpr auto Normalize(Quaternion<T> const& lhs)                               noexcept->Quaternion<T>;
		template<typename T> constexpr auto Conjugate(Quaternion<T> const& lhs)                               noexcept->Quaternion<T>;
		template<typename T> constexpr auto Inverse(Quaternion<T> const& lhs)                                 noexcept->Quaternion<T>;

		
		template<typename Src, typename Dst> constexpr auto Convert(Src const& value) noexcept->Dst;
		
		
		template<> constexpr auto Convert<Quat, Mat4x4>(Quat const& q) noexcept -> Mat4x4 {
			
			auto mat = Mat4x4{ 1.0f };
	
			auto const dxw = 2.0f * q.x * q.w;
			auto const dyw = 2.0f * q.y * q.w;
			auto const dzw = 2.0f * q.z * q.w;

			auto const dxy = 2.0f * q.x * q.y;
			auto const dxz = 2.0f * q.x * q.z;
			auto const dyz = 2.0f * q.y * q.z;

			auto const dxx = 2.0f * q.x * q.x;
			auto const dyy = 2.0f * q.y * q.y;
			auto const dzz = 2.0f * q.z * q.z;
			
			mat(0, 0) = 1.0f - dyy - dzz;
			mat(0, 1) = dxy  - dzw;
			mat(0, 2) = dxz  + dyw;

			mat(1, 0) = dxy  + dzw;
			mat(1, 1) = 1.0f - dxx - dzz;
			mat(1, 2) = dyz  - dxw;

			mat(2, 0) = dxz  - dyw;
			mat(2, 1) = dyz  + dxw;	
			mat(2, 2) = 1.0f - dxx - dyy;
			return mat;
	
		}
		
		
	}
}




namespace Hawk {

	namespace Math {


	
		template<typename T>
		[[nodiscard]] ILINE constexpr auto Radians(T degrees) noexcept -> T {
			return (degrees * PI<T>) / T{ 180 };
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Degrees(T radians) noexcept -> T {
			return  (T{ 180 } * radians) / PI<T>;
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Abs(T a) noexcept -> T {
			return std::abs(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Sin(T a) noexcept -> T {
			return std::sin(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Cos(T a) noexcept -> T {
			return std::cos(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Tan(T a) noexcept -> T {
			return std::tan(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Pow(T a) noexcept -> T {
			return std::pow(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Exp(T a) noexcept -> T {
			return std::exp(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Log(T a) noexcept -> T {
			return std::log(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Asin(T a) noexcept -> T {
			return std::asin(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Acos(T a) noexcept -> T {
			return std::acos(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Atan(T a) noexcept -> T {
			return std::atan(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Sqrt(T a) noexcept -> T {
			return std::sqrt(a);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Sign(T a) noexcept -> T {
			//Change
			return T{ 0 };
		}





		template<typename T>
		[[nodiscard]] ILINE constexpr auto Lerp(T v0, T v1, T t) noexcept -> T {
			return (T{ 1 } - t) * v0 + t * v1;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Abs(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Abs(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Sin(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Sin(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Cos(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Cos(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Tan(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Tan(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Pow(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Pow(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Exp(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Exp(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Log(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Log(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Asin(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Asin(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Acos(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Acos(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Atan(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Atan(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Sqrt(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Sqrt(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Sign(Vector<T, N> const & v) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Sign(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Lerp(Vector<T, N> const & v0, Vector<T, N> const & v1, Vector<T, N> const & t) noexcept -> Vector<T, N> {
			auto res = Vector<T, N>{};
			for (auto index = 0; index < N; index++)
				res[index] = Lerp(v[index]);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Dot(Vector<T, N> const& lhs, Vector<T, N> const& rhs) noexcept -> T {
			auto result = T{ 0 };
			for (auto index = 0u; index < N; index++)
				result += lhs[index] * rhs[index];
			return result;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Transpose(Matrix<T, N, N> const& m) noexcept -> Matrix<T, N, N> {
			Matrix<T, N, N> res;
			for (auto i = 0; i < N; i++)
				for (auto j = 0; j < N; j++)
					res(i, j) = m(j, i);
			return res;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Inverse(Matrix<T, N, N> const& m) noexcept -> Matrix<T, N, N> {

			auto M = Matrix<T, N, N>{ m };
			auto I = Matrix<T, N, N>{ T{ 1 } };


			auto temp = T{ 0 };
			for (auto i = 0; i < N; i++) {

				if (std::abs(M(i, i)) < std::numeric_limits<T>::epsilon() && i < (N - 1)) {
					M.v[i] += M.v[i + 1];
					I.v[i] += I.v[i + 1];
				}
				temp = M(i, i);
				M.v[i] /= temp;
				I.v[i] /= temp;
				for (auto j = 0; j < N; j++) {
					if (i != j) {
						temp = M(j, i);
						M.v[j] -= (temp * M.v[i]);
						I.v[j] -= (temp * I.v[i]);
					}
				}
			}
			return I;
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Length(Vector<T, N> const& v) noexcept -> T {
			return std::sqrt(Dot(v, v));
		}

		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Distance(Vector<T, N> const& lhs, Vector<T, N> const& rhs) noexcept -> T {
			return Length{ lhs - rhs };
		}


		template<typename T, U32 N>
		[[nodiscard]] ILINE constexpr auto Normalize(Vector<T, N> const& v) noexcept -> Vector<T, N> {
			auto lenght = Length<T>(v);
			return (lenght < std::numeric_limits<T>::epsilon()) ? Vector<T, N>{0} : v / lenght;
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Cross(Vec3_tpl<T> const& lhs, Vec3_tpl<T> const& rhs) noexcept-> Vec3_tpl<T> {
			return Vec3_tpl<T>(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Dot(Quaternion<T> const & lhs, Quaternion<T> const & rhs) noexcept -> T {
			auto result = T{ 0 };
			for (auto index = 0u; index < 4; index++)
				result += lhs[index] * rhs[index];
			return result;
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Normalize(Quaternion<T> const & lhs) noexcept -> Quaternion<T> {
			return lhs / Sqrt(Dot(lhs, lhs));
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Conjugate(Quaternion<T> const & lhs) noexcept -> Quaternion<T> {
			return Quaternion<T>{-lhs.x, -lhs.y, -lhs.z, lhs.w};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Inverse(Quaternion<T> const & lhs) noexcept -> Quaternion<T> {
			return Conjugate(lhs) / Dot(lhs, lhs);
		}


	}
}


