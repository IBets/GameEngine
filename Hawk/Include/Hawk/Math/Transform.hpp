#pragma once

#include <Hawk/Math/Math.hpp>

namespace Hawk {
	namespace Math {

		template<typename T> constexpr auto Rotate(Quaternion<T> const& q, Vec3_tpl<T> const& v) noexcept->Vec3_tpl<T> {
		
			auto res  = q * Quaternion<T>{v, 0.0} * Conjugate(q);

			return  Vec3_tpl<T>{res.x, res.y, res.z};
		}

		template<typename T> constexpr auto RotateX(T angle) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto RotateY(T angle) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto RotateZ(T angle) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Translate(Vec3_tpl<T> const& v)  noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Translate(T const& x, T const& y, T const& z) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Scale(Vec3_tpl<T> const& v)  noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Scale(T v) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Scale(T x, T y, T z) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Perspective(T fov, T aspect, T zNear) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto LookAt(Vec3_tpl<T> const& position, Vec3_tpl<T> const& center, Vec3_tpl<T> const& up) noexcept->Mat4x4_tpl<T>;
		template<typename T> constexpr auto AxisAngle(Vec3_tpl<T> const& v, T alpha)  noexcept->Quaternion<T>;
		template<typename T> constexpr auto EulerAngles(Vec3_tpl<T> const& v)  noexcept->Quaternion<T>;
	
	}
}


namespace Hawk {
	namespace Math {

		template<typename T>
		[[nodiscard]] ILINE constexpr auto RotateX(T angle) noexcept -> Mat4x4_tpl<T> {
			auto const c = Cos(angle);
			auto const s = Sin(angle);
			return Mat4x4_tpl<T>{
				T{ 1 }, T{ 0 }, T{ 0 }, T{ 0 },
				T{ 0 }, T{ c }, -T{ s }, T{ 0 },
				T{ 0 }, T{ s }, T{ c }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto RotateY(T angle) noexcept -> Mat4x4_tpl<T> {
			auto const c = Cos(angle);
			auto const s = Sin(angle);
			return Mat4x4_tpl<T>{
				T{ c }, T{ 0 }, T{ s }, T{ 0 },
				T{ 0 }, T{ 1 }, T{ 0 }, T{ 0 },
				-T{ s }, T{ 0 }, T{ c }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto RotateZ(T angle) noexcept -> Mat4x4_tpl<T> {
			auto c = Cos(angle);
			auto s = Sin(angle);
			return Mat4x4_tpl<T>{
				T{ c }, -T{ s }, T{ 0 }, T{ 0 },
				T{ s }, T{ c }, T{ 0 }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 1 }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Translate(Vec3_tpl<T> const& v) noexcept -> Mat4x4_tpl<T> {
			return Mat4x4_tpl<T>{
				T{ 1 }, T{ 0 }, T{ 0 }, T{ v.x },
				T{ 0 }, T{ 1 }, T{ 0 }, T{ v.y },
				T{ 0 }, T{ 0 }, T{ 1 }, T{ v.z },
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Translate(T const & x, T const & y, T const & z) noexcept -> Mat4x4_tpl<T> {
			return Translate(Vec3_tpl<T>{x, y, z});
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Scale(Vec3_tpl<T> const & v) noexcept -> Mat4x4_tpl<T> {
			return Mat4x4_tpl<T>{
				T{ v.x }, T{ 0 }, T{ 0 }, T{ 0 },
				T{ 0 }, T{ v.y }, T{ 0 }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ v.z }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Scale(T x, T y, T z) noexcept -> Mat4x4_tpl<T> {
			return  Scale(Vec3_tpl<T>{x, y, z});
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Scale(T v) noexcept -> Mat4x4_tpl<T> {
			return Scale(v, v, v);
		}

		

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Perspective(T fov, T aspect, T zNear) noexcept -> Mat4x4_tpl<T> {
	
			auto const ctg   = T{ 1 } / Tan(fov / 2);
		
			return Mat4x4_tpl<T>{
				ctg / aspect,   T{ 0 }, T{ 0 }, T{ 0 },
			    T{ 0 },  ctg,   T{ 0 }, T{ 0 },
				T{ 0 }, T{ 0 }, T{ 0 }, zNear,
				T{ 0 }, T{ 0 }, T{ 1 }, T{ 0 }};
		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept -> Mat4x4_tpl<T> {
			return Mat4x4_tpl<T>{
				T{ 2 } / (right - left), T{ 0 }, T{ 0 }, (left + right) / (left - right),
				T{ 0 }, T{ 2 } / (top - bottom), T{ 0 }, (top + bottom) / (bottom - top),
				T{ 0 }, T{ 0 }, T{ 1 } / (zNear - zFar), (zNear) / (zNear - zFar),
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};

		}

		template<typename T>
		[[nodiscard]] ILINE constexpr auto LookAt(Vec3_tpl<T> const & position, Vec3_tpl<T> const & center, Vec3_tpl<T> const & up) noexcept -> Mat4x4_tpl<T> {
			auto const f = Normalize(position - center);
			auto const s = Normalize(Cross(up, f));
			auto const u = Normalize(Cross(f, s));

			return Mat4x4_tpl<T>{
				s.x, s.y, s.z, -Dot(s, position),
				u.x, u.y, u.z, -Dot(u, position),
				f.x, f.y, f.z, -Dot(f, position),
				T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }};

		}

		template<typename T>
		[[nodiscard]] constexpr auto AxisAngle(Vec3_tpl<T> const& v, T alpha) noexcept -> Quaternion<T> {
			return Quaternion<T>{ Sin(alpha / 2) * v, Cos(alpha / 2) };
		}

		template<typename T>
		constexpr auto EulerAngles(Vec3_tpl<T> const& v) noexcept -> Quaternion<T> {
			
			auto const cv = Cos(v);
			auto const sv = Sin(v);

			assert(0);

			//No implement

			return Quaternion<T>{ };
		}

	}
}
