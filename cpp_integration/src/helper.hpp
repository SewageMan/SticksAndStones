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
#include <variant>
#include <cstring>
#include <cstdint>
#include <ciso646>
#include <bit>

#define NARROW_DESCRIPTOR(type) \
    type* descriptor() { return static_cast<type*>(descriptor_raw); } \
    const type* descriptor() const { return static_cast<const type*>(descriptor_raw); }

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

	typedef int32_t CoordinateChunks;
	typedef int32_t CoordinateBlocks;
	typedef int32_t CoordinateUnits;
	typedef float CoordinateUnitsf;
	typedef Vector2<CoordinateChunks> Vector2Chunks;    // used to store position/size in chunk grid units
	typedef Vector2<CoordinateBlocks> Vector2Blocks;    // used to store position/size in block grid units
	typedef Vector2<CoordinateUnits> Vector2Units;     // used to store position/size in pixel grid units
	typedef Vector2<CoordinateUnitsf> Vector2Unitsf;

	constexpr CoordinateUnits block_size = 32; // how big is one block in pixels
	constexpr CoordinateUnitsf block_sizef = 32;
	constexpr CoordinateBlocks chunk_size_blocks = 32;  // how big is one chunk in blocks
	constexpr CoordinateUnits chunk_size_units = chunk_size_blocks * block_size;  // how big is one chunk in pixels

	constexpr Vector2Units block_size_vec = Vector2Units(block_size, block_size);

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

	struct ByteBuffer {
		uint8_t* data;
		size_t size;
		size_t offset;
		bool owns_data;
		bool allow_resize;

		ByteBuffer() : data(nullptr), size(0), offset(0), owns_data(false), allow_resize(false) {}

		ByteBuffer(uint8_t* data, size_t size) : data(data), size(size), offset(0), owns_data(false), allow_resize(false) {}

		ByteBuffer(uint8_t* data, size_t size, size_t offset) : data(data), size(size), offset(offset), owns_data(false), allow_resize(false) {}

		ByteBuffer(size_t size) : data(new uint8_t[size]), size(size), offset(0), owns_data(true), allow_resize(false) {}

		ByteBuffer(size_t size, size_t offset) : data(new uint8_t[size]), size(size), offset(offset), owns_data(true), allow_resize(false) {}

		ByteBuffer(size_t size, size_t offset, bool allow_resize) : data(new uint8_t[size]), size(size), offset(offset), owns_data(true), allow_resize(allow_resize) {}

		ByteBuffer(size_t size, bool allow_resize) : data(new uint8_t[size]), size(size), offset(0), owns_data(true), allow_resize(allow_resize) {}

		ByteBuffer(const ByteBuffer& other) : ByteBuffer() {
			copy_from(other);
		}

		ByteBuffer& operator=(const ByteBuffer&) = delete;

		void initialise(size_t size, size_t offset = 0, bool allow_resize = false) {
			clear();
			data = new uint8_t[size];
			this->size = size;
			this->offset = offset;
			owns_data = true;
			this->allow_resize = allow_resize;
		}

		void set_values(uint8_t* data, size_t size, size_t offset = 0, bool owns_data = false, bool allow_resize = false) {
			this->data = data;
			this->size = size;
			this->offset = offset;
			this->owns_data = owns_data;
			this->allow_resize = allow_resize;
		}

		void copy_from(const ByteBuffer& other) {
			clear();
			size = other.size;
			offset = 0;
			owns_data = true;
			allow_resize = false;

			data = new uint8_t[other.size];
			std::memcpy(data, other.data, size);
		}

		void shallow_copy_from(const ByteBuffer& other) {
			clear();
			size = other.size;
			offset = 0;
			owns_data = false;
			allow_resize = false;

			data = other.data;
		}

		void resize(size_t new_size) {
			if (not allow_resize) {
				if (owns_data) {
					panic("cannot resize buffer as its resize is disabled");
				}
				else {
					panic("cannot resize a buffer that doesent own its data");
				}
			}
			if (new_size > size) {
				new_size = std::max(new_size, size * 2);
				uint8_t* new_data = new uint8_t[new_size];
				std::memcpy(new_data, data, size);
				delete[] data;
				data = new_data;
				size = new_size;
			}
			else if (new_size < size) {
				uint8_t* new_data = new uint8_t[new_size];
				std::memcpy(new_data, data, new_size);
				delete[] data;
				data = new_data;
				size = new_size;
			}
			else {
				return;
			}
		}

		template<typename T>
		void write(const T& value) {
			if constexpr (std::is_trivially_copyable_v<T>) {
				size_t required_size = offset + sizeof(T);
				if (required_size > size) {
					resize(required_size);
				}
				std::memcpy(data + offset, &value, sizeof(T));
				offset += sizeof(T);
			}
			else if (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
				size_t string_size = value.size();
				if (offset + sizeof(size_t) + string_size > size) {
					resize(offset + sizeof(size_t) + string_size);
				}
				std::memcpy(data + offset, &string_size, sizeof(size_t));
				std::memcpy(data + offset + sizeof(size_t), value.data(), string_size);
				offset += sizeof(size_t) + string_size;
			}
			else {
				static_assert(sizeof(T) == 0, "T must be trivially copyable");
			}
		}

		template<typename T>
		void read(T& value) {
			if constexpr (std::is_trivially_copyable_v<T>) {
				if (offset + sizeof(T) > size) {
					panic("not enough bytes to read from byte buffer");
				}
				std::memcpy(&value, data + offset, sizeof(T));
				offset += sizeof(T);
			}
			else if (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
				if (offset + sizeof(size_t) > size) {
					panic("not enough bytes to read from byte buffer");
				}
				size_t string_size;
				std::memcpy(&string_size, data + offset, sizeof(size_t));
				if (offset + sizeof(size_t) + string_size > size) {
					panic("not enough bytes to read from byte buffer");
				}
				value.assign(reinterpret_cast<const char*>(data + offset + sizeof(size_t)), string_size);
				offset += sizeof(size_t) + string_size;
			}
			else {
				static_assert(sizeof(T) == 0, "T must be trivially copyable");
			}
		}

		void write_bytes(void* value, size_t value_size) {
			size_t required_size = offset + value_size;
			if (required_size > size) {
				resize(required_size);
			}
			std::memcpy(data + offset, value, value_size);
			offset += value_size;
		}

		void read_bytes(void* value, size_t value_size) {
			if (offset + value_size > size) {
				panic("not enough bytes to read from byte buffer");
			}
			std::memcpy(value, data + offset, value_size);
			offset += value_size;
		}

		void copy_from_bytes(ByteBuffer& other, size_t chunk_size) {
			size_t required_size = offset + chunk_size;
			if (required_size > size) {
				resize(required_size);
			}
			size_t required_size_other = other.offset + chunk_size;
			if (required_size_other > other.size) {
				other.resize(required_size_other);
			}
			std::memcpy(data + offset, other.data + other.offset, chunk_size);
			offset += chunk_size;
		}

		void clear() {
			if (owns_data) {
				delete[] data;
			}
			data = nullptr;
			size = 0;
			offset = 0;
			owns_data = false;
			allow_resize = false;
		}

		bool is_valid() const {
			return data != nullptr;
		}

		size_t left_bytes() const {
			return size - offset;
		}

		~ByteBuffer() {
			if (owns_data) {
				delete[] data;
			}
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const ByteBuffer& buffer) {
		os << "ByteBuffer(" << static_cast<void*>(buffer.data) << ", " << buffer.size << ", " << buffer.offset << ", " << buffer.owns_data << ", " << buffer.allow_resize << ")";
		return os;
	}

	template <typename StorageType,typename IndexType>
	struct PersistentContainer{

		std::vector<StorageType> end_storage;
		std::vector<IndexType> storage_indexes;
		std::vector<IndexType> element_indexes;
		std::vector<IndexType> generations;
		IndexType free_storage_id = 0;

		IndexType add_element(StorageType element) {
			if (free_storage_id != storage_indexes.size()) {
				IndexType storage_id = free_storage_id;
				IndexType element_id = element_indexes[free_storage_id];
				free_storage_id += 1;

				end_storage[storage_id] = element;
				return element_id;
			}
			else {
				IndexType storage_id = end_storage.size();
				IndexType element_id = storage_id;
				free_storage_id += 1;

				element_indexes.push_back(element_id);
				storage_indexes.push_back(storage_id);
				generations.push_back(0);
				end_storage.push_back(element);
				return element_id;
			}
		}

		void add_elements(std::span<StorageType> elements) {
			IndexType old_size = size();
			resize(old_size + elements.size());
			for (IndexType add_id = 0; add_id < elements.size(); ++add_id) {
				end_storage[old_size + add_id] = elements[add_id];
			}
		}

		template<typename... T>
		IndexType emplace_element(T&&... args) {
			if (free_storage_id != storage_indexes.size()) {
				IndexType storage_id = free_storage_id;
				IndexType element_id = element_indexes[free_storage_id];
				free_storage_id += 1;

				end_storage[storage_id] = StorageType(std::forward<T>(args)...);
				return element_id;
			}
			else {
				IndexType storage_id = end_storage.size();
				IndexType element_id = storage_id;
				free_storage_id += 1;

				element_indexes.push_back(element_id);
				storage_indexes.push_back(storage_id);
				generations.push_back(0);
				end_storage.emplace_back(std::forward<T>(args)...);
				return element_id;
			}
		}

		void set_element(IndexType element_id, StorageType element) {
			end_storage[element_id] = element;
		}

		StorageType get_element(IndexType element_id) {
			return end_storage[element_id];
		}

		void delete_element(IndexType element_id) {
			IndexType storage_id = storage_indexes[element_id];

			IndexType last_storage_id = free_storage_id - 1;
			if (storage_id != last_storage_id) {
				IndexType last_element_id = element_indexes[last_storage_id];
				end_storage[storage_id] = std::move(end_storage[last_storage_id]);
				std::swap(element_indexes[storage_id], element_indexes[last_storage_id]);
				std::swap(storage_indexes[element_id], storage_indexes[last_element_id]);
			}
			generations[element_id] += 1;
			free_storage_id = last_storage_id;
		}

		IndexType size() const {
			return free_storage_id;
		}

		IndexType capacity() const {
			return end_storage.size();
		}

		void reserve(IndexType to_reserve) {
			IndexType old_capacity = capacity();
			if (to_reserve <= old_capacity) {
				return;
			}
			end_storage.resize(to_reserve);
			element_indexes.resize(to_reserve);
			storage_indexes.resize(to_reserve);
			generations.resize(to_reserve, 0);
			for (IndexType add_id = old_capacity; add_id < to_reserve; ++add_id) {
				element_indexes[add_id] = add_id;
				storage_indexes[add_id] = add_id;
			}
		}

		void resize(IndexType to_resize) {
			IndexType old_size = size();
			if (to_resize <= old_size) {
				return;
			}
			free_storage_id = to_resize;
			if (to_resize <= capacity()) {
				return;
			}
			end_storage.resize(to_resize);
			element_indexes.resize(to_resize);
			storage_indexes.resize(to_resize);
			generations.resize(to_resize, 0);
			for (IndexType add_id = old_size; add_id < to_resize; ++add_id) {
				element_indexes[add_id] = add_id;
				storage_indexes[add_id] = add_id;
			}
		}

		StorageType& operator[](IndexType element_id) {
			return end_storage[storage_indexes[element_id]];
		}

		const StorageType& operator[](IndexType element_id) const {
			return end_storage[storage_indexes[element_id]];
		}

		using iterator = typename std::vector<StorageType>::iterator;
		using const_iterator = typename std::vector<StorageType>::const_iterator;

		iterator begin() {
			return end_storage.begin();
		}

		iterator end() {
			return end_storage.begin() + free_storage_id;
		}

		const_iterator begin() const {
			return end_storage.begin();
		}

		const_iterator end() const {
			return end_storage.begin() + free_storage_id;
		}

		using reverse_iterator = typename std::vector<StorageType>::reverse_iterator;
		reverse_iterator rbegin() {
			return end_storage.rbegin() + (end_storage.size() - free_storage_id);
		}
		reverse_iterator rend() {
			return end_storage.rend();
		}

		struct SnapshotIterator {
			const PersistentContainer* container;
			std::vector<std::pair<IndexType, IndexType>> snapshot;
			size_t index;

			SnapshotIterator(const PersistentContainer& container) : container(&container), index(0) {
				snapshot.reserve(container.free_storage_id);
				for (IndexType storage_id = 0; storage_id < container.free_storage_id; ++storage_id) {
					IndexType element_id = container.element_indexes[storage_id];
					snapshot.emplace_back(element_id, container.generations[element_id]);
				}
				if (snapshot.empty()) {
					this->container = nullptr;
				}
			}

			SnapshotIterator() : container(nullptr), index(0) {}

			bool is_valid() const {
				if (index >= snapshot.size()) return false;
				auto [id, gen] = snapshot[index];
				return container->generations[id] == gen;
			}

			void skip_invalid() {
				while (index < snapshot.size() && !is_valid()) {
					++index;
				}
			}

			StorageType operator*() const {
				auto [id, gen] = snapshot[index];
				IndexType storage_id = container->storage_indexes[id];
				return container->end_storage[storage_id];
			}

			SnapshotIterator& operator++() {
				++index;
				skip_invalid();
				if (index >= snapshot.size()) {
					container = nullptr;
					index = 0;
				}
				return *this;
			}

			bool operator!=(const SnapshotIterator& other) const {
				return container != other.container || index != other.index;
			}

			SnapshotIterator begin() const { return *this; }
			SnapshotIterator end() const { return SnapshotIterator(); }
		};

		struct SnapshotRange {
			const PersistentContainer& container;
			SnapshotRange(const PersistentContainer& c) : container(c) {}

			SnapshotIterator begin() const { return SnapshotIterator(container); }
			SnapshotIterator end() const { return SnapshotIterator(); }
		};

		SnapshotRange snapshot() const {
			return SnapshotRange(*this);
		}
	};

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
}