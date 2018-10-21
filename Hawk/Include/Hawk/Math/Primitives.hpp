#pragma once

#include "./Math.hpp"

namespace Hawk {
	namespace Math {


		class Sphere {
		public:
			constexpr Sphere(Math::Vec3 const& center, F32 radius) noexcept;

			constexpr auto SetCenter(Math::Vec3 const &v) noexcept->void;
			constexpr auto SetCenter(F32 x, F32 y, F32 z) noexcept->void;
			constexpr auto SetRadius(F32 &r)              noexcept->void;

			constexpr auto Center() const noexcept->Math::Vec3 const&;
			constexpr auto Radius() const noexcept->F32;

		private:
			Math::Vec3 m_Center;
			F32        m_Radius;
		};

		class AABB {
		public:
			constexpr AABB(Math::Vec3 const& center, F32 radius) noexcept;
			constexpr auto SetMinExtends(Math::Vec3 const &v) noexcept->void;
			constexpr auto SetMaxExtends(Math::Vec3 const &v) noexcept->void;
			constexpr auto SetMinExtends(F32 x, F32 y, F32 z) noexcept->void;
			constexpr auto SetMaxExtends(F32 x, F32 y, F32 z) noexcept->void;

			constexpr auto MinExtends() const noexcept->Math::Vec3 const&;
			constexpr auto MaxExtends() const noexcept->Math::Vec3 const&;

		private:
			Math::Vec3 m_MinExtends;
			Math::Vec3 m_MaxExtends;

		};


	}
}
