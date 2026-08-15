#pragma once

#include <vector.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <stacktrace>
#include <string>
#include <exception>
#include <filesystem>
#include <unordered_map>
#include <string_view>
#include <array>
#include <fstream>
#include <sstream>
#include <optional>
#include <span>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <limits>
#include <random>
#include <format>
#include <streambuf>
#include <concepts>

namespace engine {

	enum Direction {
		TOP = 0,
		RIGHT = 1,
		BOTTOM = 2,
		LEFT = 3
	};

	template<typename T, int rows, int collums>
	using Matrix2D = std::array<std::array<T, rows>, collums>;

	typedef uint32_t RefCounter;

	typedef double Seconds;

	typedef int32_t pos_t;           // used to store all sizes and positions in world for individual pixels
	typedef Vector2<pos_t> Vector2p; // used to store a pixel point position in world or size of something
	typedef Vector2p Vector2Chunk;   // used to store position/size in chunk grid units
	typedef Vector2p Vector2Block;   // used to store position/size in block grid units
	typedef Vector2p Vector2Units;   // used to store position/size in pixel grid units

	constexpr pos_t block_size = 32; // how big is one block in pixels
	constexpr pos_t chunk_size_blocks = 16;  // how big is one chunk in blocks
	constexpr pos_t chunk_size_units = chunk_size_blocks * block_size;  // how big is one chunk in pixels

	Vector2Units block_size_vec = Vector2Units(block_size, block_size);

	template<typename T>
	using ChunkMatrix = Matrix2D<T,chunk_size_blocks,chunk_size_blocks>;  // ChunkMatrix[x][y] is correct indexing order

	static std::ostream* out_stream = nullptr;

	template <typename T>
	std::string to_string_ptr(T* pointer) {
		return std::format("{:p}", static_cast<const void*>(pointer));
	}

	template <typename T>
	bool can_find_in_vec(std::vector<T>& vector,T value) {
		auto found = std::find(vector.begin(), vector.end(), value);
		return found != vector.end();
	}

	template<typename... T>
	void print(T... args) {
		(..., (*out_stream << args << " "));
		out_stream->flush();
	}

	class CriticalExceptionStopGodot : public std::runtime_error {
	public:
		CriticalExceptionStopGodot(const std::string& msg) : std::runtime_error(msg) {}
	};

	void panic(const std::string& message) {
		throw CriticalExceptionStopGodot(message + "\n\n" + std::to_string(std::stacktrace::current()));
	}

	void panic() {
		panic("no panic message was set");
	}

	void panic_path(const std::string& message, const std::string& path) {
		panic(message + ", at path: '" + path + "'");
	}

	bool try_parse_int(const std::string& str, int& out_value) {
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), out_value);

		return ec == std::errc();
	}

	std::string read_file(const std::string& path) {
		std::ifstream file(path);
		if (!file.is_open()) {
			panic_path("ERROR: Could not open file", path);
		}
		std::stringstream stream;
		stream << file.rdbuf();
		return stream.str();
	}

	template <typename T>
	struct BoundingBox {

		Vector2<T> pos;
		Vector2<T> half_size;

		BoundingBox() {}

		BoundingBox(Vector2<T> pos, Vector2<T> size) :pos(pos), half_size(size / 2) {}

		void set(Vector2<T> pos, Vector2<T> size) {
			this->pos = pos;
			this->half_size = size / 2;
		}

		void set_corner(Vector2<T> pos, Vector2<T> size) {
			this->half_size = size / 2;
			this->pos = pos + half_size;
		}

		template <uint32_t side>
		T get_side() { // 0 - top, 1-right, 2-bottom, 3-left, x+ is right, y+ is top
			if constexpr (side==0) {
				return pos.y + half_size.y;
			}
			else if constexpr (side == 1) {
				return pos.x + half_size.x;
			}
			else if constexpr (side == 2) {
				return pos.y - half_size.y;
			}
			else if constexpr (side == 3) {
				return pos.x - half_size.x;
			}
			else {
				static_assert(true, "unexpected mode for get side, only accept 0 to 3 modes");
			}
		}

		BoundingBox corner_cut(Vector2<T> size) {
			BoundingBox result;
			result.set_corner(corner(), size);
			return result;
		}

		Vector2<T> corner() {
			return pos - half_size;
		}

		Vector2<T> size() {
			return half_size * 2;
		}

		bool is_colliding(const BoundingBox<T> other) const {
			return std::abs(pos.x - other.pos.x) <= (half_size.x + other.half_size.x) and std::abs(pos.y - other.pos.y) <= (half_size.y + other.half_size.y);
		}

		bool is_colliding(const Vector2<T> pos) const {
			return std::abs(pos.x - this->pos.x) <= half_size.x and std::abs(pos.y - this->pos.y) <= half_size.y;
		}

	};

	template <typename T>
	struct CBoundingBox {

		Vector2<T> pos;
		Vector2<T> size;

		CBoundingBox() {}

		CBoundingBox(Vector2<T> pos, Vector2<T> size) :pos(pos), size(size) {}

		void set(Vector2<T> pos, Vector2<T> size) {
			this->pos = pos;
			this->size = size;
		}

		template <uint32_t side>
		void set_side(T value) { // 0 - top, 1-right, 2-bottom, 3-left, x+ is right, y+ is top
			if constexpr (side == 0) {
				set_top(value);
			}
			else if constexpr (side == 1) {
				set_right(value);
			}
			else if constexpr (side == 2) {
				set_bottom(value);
			}
			else if constexpr (side == 3) {
				set_left(value);
			}
			else {
				static_assert(true, "unexpected mode for set side, only accept 0 to 3 modes");
			}
		}

		void set_side(T value, uint32_t side) { // 0 - top, 1-right, 2-bottom, 3-left, x+ is right, y+ is top
			if (side == 0) {
				set_top(value);
			}
			else if (side == 1) {
				set_right(value);
			}
			else if (side == 2) {
				set_bottom(value);
			}
			else if (side == 3) {
				set_left(value);
			}
			else {
				panic("unexpected mode for set side, only accept 0 to 3 modes");
			}
		}

		void set_top(T value) {
			size.y = value - pos.y + static_cast<T>(1);
		}

		void set_right(T value) {
			size.x = value - pos.x + static_cast<T>(1);
		}

		void set_bottom(T value) {
			size.y += pos.y - value;
			pos.y = value;
		}

		void set_left(T value) {
			size.x += pos.x - value;
			pos.x = value;
		}

		CBoundingBox corner_cut(Vector2<T> size) const {
			return CBoundingBox(this->pos, size);
		}

		template <bool vertical>
		bool fits_across(uint32_t offset) const {
			if constexpr (vertical) {
				return left() <= offset and offset <= right();
			}
			else {
				return bottom() <= offset and offset <= top();
			}
		}

		template <bool vertical, bool only_right_check>
		bool fits_across(uint32_t offset) const {
			if constexpr (vertical) {
				if constexpr (only_right_check) {
					return offset <= right();
				}
				else {
					return offset >= left();
				}
			}
			else {
				if constexpr (only_right_check) {
					return offset >= bottom();
				}
				else {
					return offset <= top();
				}
			}
		}

		template <uint32_t side>
		T get_side() const { // 0 - top, 1-right, 2-bottom, 3-left, x+ is right, y+ is top
			if constexpr (side == 0) {
				return top();
			}
			else if constexpr (side == 1) {
				return right();
			}
			else if constexpr (side == 2) {
				return bottom();
			}
			else if constexpr (side == 3) {
				return left();
			}
			else {
				static_assert(true, "unexpected mode for get side, only accept 0 to 3 modes");
			}
		}

		T get_side(uint32_t side) const { // 0 - top, 1-right, 2-bottom, 3-left, x+ is right, y+ is top
			if (side == 0) {
				return top();
			}
			else if (side == 1) {
				return right();
			}
			else if (side == 2) {
				return bottom();
			}
			else if (side == 3) {
				return left();
			}
			else {
				panic("unexpected mode for get side, only accept 0 to 3 modes");
			}
		}

		T top() const {
			return pos.y + size.y - static_cast<T>(1);
		}

		T right() const {
			return pos.x + size.x - static_cast<T>(1);
		}

		T bottom() const {
			return pos.y;
		}
		
		T left() const {
			return pos.x;
		}

		T height() const {
			return size.y;
		}

		T width() const {
			return size.x;
		}

		Vector2<T> bottom_left() {
			return pos;
		}

		Vector2<T> bottom_right() {
			return { pos.x + size.x - 1, pos.y };
		}

		Vector2<T> top_left() {
			return { pos.x, pos.y + size.y - 1 };
		}

		Vector2<T> top_right() {
			return { pos.x + size.x - 1, pos.y + size.y - 1 };
		}

		bool is_colliding(const CBoundingBox<T> other) const {
			return left() <= other.right() and right() >= other.left() and bottom() <= other.top() and top() >= other.bottom();
		}

		bool is_colliding(const Vector2<T> pos) const {
			return left() <= pos.x <= right() and bottom() <= pos.y <= top();
		}

		bool contains(const CBoundingBox& other) const {
			return other.left() >= left() and other.right() <= right() and other.bottom() >= bottom() and other.top() <= top();
		}

		std::string to_string() const {
			return "CBBox(" + pos.to_string() + ", " + size.to_string() + ")";
		}

	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const CBoundingBox<T> bbox) {
		os << bbox.to_string();
		return os;
	}

	template <typename T>
	void reserve_add(std::vector<T>& vec, size_t amount) {
		vec.reserve(vec.size() + amount);
	}

	template <typename T>
	void reserve_exponentialy(std::vector<T>& vec, size_t amount) {

		if (amount > vec.capacity()) {

			size_t newCapacity = vec.capacity() * 2;

			if (newCapacity < amount) {
				newCapacity = amount;
			}
			vec.reserve(newCapacity);
		}
	}

	template <typename T>
	void reserve_add_exponentialy(std::vector<T>& vec, size_t amount) {
		reserve_exponentialy(vec, vec.size() + amount);
	}

	
}