#pragma once

#include <string>
#include <stdexcept>
#include <cmath>
#include <all_nums.hpp>
#include <vector>
#include <concepts>
#include <functional>


namespace engine {
	template<typename U>
	class Range2;

	template <typename T>
	struct Vector2 {
		T x;
		T y;

		Vector2() {}

		Vector2(T x_val, T y_val) : x(x_val), y(y_val) {}

		Vector2<T> going_to(const Vector2<T> other) const {
			return other - (*this);
		}

		template <typename U>
		auto get_len() const {
			U dx = static_cast<U>(x);
			U dy = static_cast<U>(y);
			return std::sqrt(dx * dx + dy * dy);
		}

		auto get_len() const {
			using FloatType = std::conditional_t<std::is_floating_point_v<T>, T, double>;
			FloatType dx = static_cast<FloatType>(x);
			FloatType dy = static_cast<FloatType>(y);
			return std::sqrt(dx * dx + dy * dy);
		}

		template <typename U>
		auto get_len2() const {
			U dx = static_cast<U>(x);
			U dy = static_cast<U>(y);
			return dx * dx + dy * dy;
		}

		auto get_len2() const {
			using FloatType = std::conditional_t<std::is_floating_point_v<T>, T, double>;
			FloatType dx = static_cast<FloatType>(x);
			FloatType dy = static_cast<FloatType>(y);
			return dx * dx + dy * dy;
		}

		Vector2<T> get_normalize() const {
			return (*this) / get_len();
		}

		void normalize() {
			(*this)/=get_len();
		}

		Vector2<T> with_len(all_nums auto len) const {
			return (*this) * len / get_len();
		}

		void set_len(all_nums auto len) {
			(*this) *= len/get_len();
		}

		Vector2<T> get_rotate(all_nums auto angle) const {
			double sin_a = std::sin(angle);
			double cos_a = std::cos(angle);
			return Vector2<T>(x * cos_a - y * sin_a, x * sin_a + y * cos_a);
		}

		void rotate(all_nums auto angle) {
			double sin_a = std::sin(angle);
			double cos_a = std::cos(angle);
			T old_x = x;
			T old_y = y;
			x = old_x * cos_a - old_y * sin_a;
			y = old_x * sin_a + old_y * cos_a;
		}

		Vector2<T> get_rotate_around(const Vector2<T> center, all_nums auto angle) const {
			return ((*this) - center).get_rotate(angle) + center;
		}

		void rotate_around(const Vector2<T> center, all_nums auto angle) {
			(*this) -= center;
			(*this)->rotate(angle);
			(*this) += center;
		}

		Vector2<T> get_rotate90() const {
			return Vector2<T>(-y, x);
		}

		Vector2<T> get_rotate180() const {
			return Vector2<T>(-x, -y);
		}

		Vector2<T> get_rotate270() const {
			return Vector2<T>(y, -x);
		}

		void rotate90() {
			T old_x = x;
			x = -y;
			y = old_x;
		}

		void rotate180() {
			x = -x;
			y = -y;
		}

		void rotate270() {
			T old_x = x;
			x = y;
			y = -old_x;
		}

		Vector2<T> get_flip_x() const {
			return Vector2<T>(-x, y);
		}

		Vector2<T> get_flip_y() const {
			return Vector2<T>(x, -y);
		}

		Vector2<T> get_flip() const {
			return Vector2<T>(-x, -y);
		}

		void flip_x() {
			x = -x;
		}

		void flip_y() {
			y = -y;
		}

		void flip() {
			x = -x;
			y = -y;
		}

		auto normalised_cross(const Vector2<T> other) const {
			using FloatType = std::conditional_t<std::is_floating_point_v<T>, T, double>;
			FloatType dx1 = static_cast<FloatType>(x);
			FloatType dy1 = static_cast<FloatType>(y);
			FloatType dx2 = static_cast<FloatType>(other.x);
			FloatType dy2 = static_cast<FloatType>(other.y);

			FloatType len_result = get_len2() * other.get_len2();
			if (len_result<1e-12f) {
				return static_cast<FloatType>(0);
			}

			return (dx1 * dy2 - dy1 * dx2) / std::sqrt(len_result);
		}

		template <typename U>
		Vector2<U> get_discretize(all_nums auto step) const {
			return Vector2<U>(std::floor(x / step), std::floor(y / step));
		}

		template <typename U>
		void discretize(all_nums auto step) {
			x = std::floor(x / step);
			y = std::floor(y / step);
		}

		auto get_distance_to(const Vector2<T> other) const {
			return going_to(other).get_len();
		}

		auto get_distance2_to(const Vector2<T> other) const {
			return going_to(other).get_len2();
		}

		auto get_angle_to(const Vector2<T> other) const {
			return std::atan2((*this) ^ other, (*this) * other);
		}

		auto get_angle() const {
			return std::atan2(y, x);
		}

		Vector2<T> get_abs() const {
			return Vector2<T>(std::abs(x), std::abs(y));
		}

		void abs() {
			x = std::abs(x);
			y = std::abs(y);
		}

		template <typename U>
		std::vector<Vector2<U>> range_to(const Vector2<T> other,all_nums auto step) const {
			Vector2<U> start = static_cast<Vector2<U>>((*this).get_discretize<U>(step));
			Vector2<U> end = static_cast<Vector2<U>>(other.get_discretize<U>(step));
			Vector2<U> diff = start - end;
			std::vector<Vector2<U>> res_list;
			res_list.reserve(static_cast<size_t>(diff.x) * static_cast<size_t>(diff.y));
			for (U y_i = start.y; y_i <= end.y; ++y_i) {
				for (U x_i = start.x; x_i <= end.x; ++x_i) {
					res_list.emplace_back(x_i,y_i);
				}
			}
			return res_list;
		}

		template <typename U>
		Range2<U> range_to_iter(const Vector2<T> other, all_nums auto step) const {
			Vector2<U> start = static_cast<Vector2<U>>((*this).get_discretize<U>(step));
			Vector2<U> end = static_cast<Vector2<U>>(other.get_discretize<U>(step));
			return Range2<U>(start.x,start.y,end.x,end.y);
		}

		bool is_infinite() const {
			return std::isinf(x) or std::isinf(y);
		}

		Vector2<T> get_round() const {
			return Vector2<T>(std::round(x), std::round(y));
		}

		Vector2<T> get_floor() const {
			return Vector2<T>(std::floor(x), std::floor(y));
		}

		Vector2<T> get_ceil() const {
			return Vector2<T>(std::ceil(x), std::ceil(y));
		}

		void round() {
			x = std::round(x);
			y = std::round(y);
		}

		void floor() {
			x = std::floor(x);
			y = std::floor(y);
		}

		void ceil() {
			x = std::ceil(x);
			y = std::ceil(y);
		}

		template<typename U>
		Vector2<T> operator+=(const Vector2<U>& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		template<typename U>
		Vector2<T> operator-=(const Vector2<U>& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2<T> operator*=(all_nums auto scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector2<T> operator/=(all_nums auto scalar) {
			x /= scalar;
			y /= scalar;
			return *this;
		}

		T operator[](size_t index) const {
			if (index == 0) {
				return x;
			}
			else if (index == 1) {
				return y;
			}
			else {
				throw std::out_of_range("Vector2 index out of bounds: " + std::to_string(index));
			}
		}

		std::string to_string() const {
			return "vec(" + std::to_string(x) + ", " + std::to_string(y) + ")";
		}

		template <typename S>
		explicit Vector2(const Vector2<S> other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {
		}
	};

	template<typename T, typename U>
	inline auto operator+(const Vector2<T> lhs, const Vector2<U> rhs) {
		using ResultType = decltype(lhs.x + rhs.x);
		return Vector2<ResultType>(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	template<typename T>
	inline auto operator+(const Vector2<T> vec) {
		return Vector2<T>(vec.x, vec.y);
	}

	template<typename T, typename U>
	inline auto operator-(const Vector2<T> lhs, const Vector2<U> rhs) {
		using ResultType = decltype(lhs.x - rhs.x);
		return Vector2<ResultType>(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	template<typename T>
	inline auto operator-(const Vector2<T> vec) {
		return Vector2<T>(-vec.x, -vec.y);
	}

	template<typename T, typename U>
	inline auto operator^(const Vector2<T> lhs, const Vector2<U> rhs) {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	template<typename T, typename U>
	inline auto operator*(const Vector2<T> lhs, const Vector2<U> rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template<typename T, typename S>
	inline auto operator*(const Vector2<T> vec, S scalar) {
		using ResultType = decltype(vec.x* scalar);
		return Vector2<ResultType>(vec.x * scalar, vec.y * scalar);
	}

	template<typename T, typename S>
	inline auto operator*(S scalar, const Vector2<T> vec) {
		using ResultType = decltype(vec.x* scalar);
		return Vector2<ResultType>(vec.x * scalar, vec.y * scalar);
	}

	template<typename T, typename S>
	inline auto operator/(const Vector2<T> vec, S scalar) {
		using ResultType = decltype(vec.x / scalar);
		return Vector2<ResultType>(vec.x / scalar, vec.y / scalar);
	}

	template<typename T, typename U>
	inline auto operator==(const Vector2<T> lhs, const Vector2<U> rhs) {
		return lhs.x == rhs.x and lhs.y == rhs.y;
	}

	template<typename T, typename U>
	inline auto operator!=(const Vector2<T> lhs, const Vector2<U> rhs) {
		return lhs.x != rhs.x or lhs.y != rhs.y;
	}

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Vector2<T> vec) {
		os << vec.to_string();
		return os;
	}

	template<typename U>
	class Range2 {
		U min_x, max_x;
		U min_y, max_y;

	public:
		Range2(U min_x, U min_y, U max_x, U max_y)
			: min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y) {
		}

		class iterator {
			U current_x, current_y;
			U min_x, max_x;

		public:
			iterator(U x, U y, U min_x, U max_x)
				: current_x(x), current_y(y), min_x(min_x), max_x(max_x) {
			}

			Vector2<U> operator*() const {
				return Vector2<U>(current_x, current_y);
			}

			iterator& operator++() {
				++current_x;
				if (current_x > max_x) {
					current_x = min_x;
					++current_y;
				}
				return *this;
			}

			bool operator!=(const iterator& other) const {
				return current_x != other.current_x or current_y != other.current_y;
			}
		};

		iterator begin() const {
			return iterator(min_x, min_y, min_x, max_x);
		}

		iterator end() const {
			return iterator(min_x, max_y + 1, min_x, max_x);
		}
	};
}

namespace std {
	template<typename T>
	struct hash<engine::Vector2<T>> {
		size_t operator()(const engine::Vector2<T>& v) const {
			T nx = v.x;
			T ny = v.y;

			if constexpr (std::floating_point<T>) {
				if (nx == 0.0) nx = 0.0;
				if (ny == 0.0) ny = 0.0;
			}

			size_t h1 = std::hash<T>{}(nx);
			size_t h2 = std::hash<T>{}(ny);

			h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);

			return h1;
		}
	};
}