#pragma once

#include <godot_helper.hpp>

namespace godot {

    struct GodotByteBuffer : Object {
        GDCLASS(GodotByteBuffer, Object)

    public:
        engine::ByteBuffer byte_buffer;

        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("write_bool", "value"), &GodotByteBuffer::write_bool);
            ClassDB::bind_method(D_METHOD("read_bool"), &GodotByteBuffer::read_bool);
            ClassDB::bind_method(D_METHOD("write_uint8", "value"), &GodotByteBuffer::write_uint8);
            ClassDB::bind_method(D_METHOD("read_uint8"), &GodotByteBuffer::read_uint8);
            ClassDB::bind_method(D_METHOD("write_uint32", "value"), &GodotByteBuffer::write_uint32);
            ClassDB::bind_method(D_METHOD("read_uint32"), &GodotByteBuffer::read_uint32);
            ClassDB::bind_method(D_METHOD("write_int32", "value"), &GodotByteBuffer::write_int32);
            ClassDB::bind_method(D_METHOD("read_int32"), &GodotByteBuffer::read_int32);
            ClassDB::bind_method(D_METHOD("write_int64", "value"), &GodotByteBuffer::write_int64);
            ClassDB::bind_method(D_METHOD("read_int64"), &GodotByteBuffer::read_int64);
            ClassDB::bind_method(D_METHOD("write_float", "value"), &GodotByteBuffer::write_float);
            ClassDB::bind_method(D_METHOD("read_float"), &GodotByteBuffer::read_float);
            ClassDB::bind_method(D_METHOD("write_double", "value"), &GodotByteBuffer::write_double);
            ClassDB::bind_method(D_METHOD("read_double"), &GodotByteBuffer::read_double);
            ClassDB::bind_method(D_METHOD("write_string", "value"), &GodotByteBuffer::write_string);
            ClassDB::bind_method(D_METHOD("read_string"), &GodotByteBuffer::read_string);
            ClassDB::bind_method(D_METHOD("write_vector2f", "value"), &GodotByteBuffer::write_vector2f);
            ClassDB::bind_method(D_METHOD("read_vector2f"), &GodotByteBuffer::read_vector2f);
            ClassDB::bind_method(D_METHOD("write_vector2i", "value"), &GodotByteBuffer::write_vector2i);
            ClassDB::bind_method(D_METHOD("read_vector2i"), &GodotByteBuffer::read_vector2i);
            ClassDB::bind_method(D_METHOD("write_buffer", "buffer"), &GodotByteBuffer::write_buffer);
            ClassDB::bind_method(D_METHOD("read_buffer"), &GodotByteBuffer::read_buffer);
            ClassDB::bind_method(D_METHOD("reference_data", "buffer"), &GodotByteBuffer::reference_data);
            ClassDB::bind_method(D_METHOD("dereference_data", "size", "offset"), &GodotByteBuffer::dereference_data, DEFVAL(0));
            ClassDB::bind_method(D_METHOD("initialise", "size", "offset", "allow_resize"), &GodotByteBuffer::initialise, DEFVAL(0), DEFVAL(0), DEFVAL(false));
            ClassDB::bind_method(D_METHOD("copy"), &GodotByteBuffer::copy);
            ClassDB::bind_method(D_METHOD("copy_from", "other"), &GodotByteBuffer::copy_from);
            ClassDB::bind_method(D_METHOD("shallow_copy_from", "other"), &GodotByteBuffer::shallow_copy_from);
            ClassDB::bind_method(D_METHOD("copy_from_bytes", "other", "chunk_size"), &GodotByteBuffer::copy_from_bytes);
            ClassDB::bind_method(D_METHOD("resize", "new_size"), &GodotByteBuffer::resize);
            ClassDB::bind_method(D_METHOD("get_size"), &GodotByteBuffer::get_size);
            ClassDB::bind_method(D_METHOD("set_allow_resize", "new_allow_resize"), &GodotByteBuffer::set_allow_resize);
            ClassDB::bind_method(D_METHOD("get_allow_resize"), &GodotByteBuffer::get_allow_resize);
            ClassDB::bind_method(D_METHOD("set_offset", "new_offset"), &GodotByteBuffer::set_offset);
            ClassDB::bind_method(D_METHOD("get_offset"), &GodotByteBuffer::get_offset);
            ClassDB::bind_method(D_METHOD("get_owns_data"), &GodotByteBuffer::get_owns_data);
            ClassDB::bind_method(D_METHOD("is_valid"), &GodotByteBuffer::is_valid);
            ClassDB::bind_method(D_METHOD("clear"), &GodotByteBuffer::clear);
            ClassDB::bind_method(D_METHOD("print_out"), &GodotByteBuffer::print_out);
            ClassDB::bind_method(D_METHOD("deallocate"), &GodotByteBuffer::deallocate);
            ClassDB::bind_static_method("GodotByteBuffer", D_METHOD("allocate"), &GodotByteBuffer::allocate_godot_byte_buffer);
        }

        void write_bool(bool value) {
            bool typed_value = value;
            byte_buffer.write<bool>(typed_value);
        }

        bool read_bool() {
            bool result;
            byte_buffer.read<bool>(result);
            return result;
        }

        void write_uint8(uint64_t value) {
            uint8_t typed_value = value;
            byte_buffer.write<uint8_t>(typed_value);
        }

        int64_t read_uint8() {
            uint8_t result;
            byte_buffer.read<uint8_t>(result);
            return result;
        }

        void write_uint32(int64_t value) {
            uint32_t typed_value = value;
            byte_buffer.write<uint32_t>(typed_value);
        }

        int64_t read_uint32() {
            uint32_t result;
            byte_buffer.read<uint32_t>(result);
            return result;
        }

        void write_int32(int64_t value) {
            int32_t typed_value = value;
            byte_buffer.write<int32_t>(typed_value);
        }

        int64_t read_int32() {
            int32_t result;
            byte_buffer.read<int32_t>(result);
            return result;
        }

        void write_int64(int64_t value) {
            int64_t typed_value = value;
            byte_buffer.write<int64_t>(typed_value);
        }

        int64_t read_int64() {
            int64_t result;
            byte_buffer.read<int64_t>(result);
            return result;
        }

        void write_vector2f(Vector2 value) {
            engine::Vector2f typed_value = { value.x, value.y };
            byte_buffer.write<engine::Vector2f>(typed_value);
        }

        void write_float(float value) {
            float typed_value = value;
            byte_buffer.write<float>(typed_value);
        }

        float read_float() {
            float result;
            byte_buffer.read<float>(result);
            return result;
        }

        void write_double(double value) {
            double typed_value = value;
            byte_buffer.write<double>(typed_value);
        }

        double read_double() {
            double result;
            byte_buffer.read<double>(result);
            return result;
        }

        String read_string() {
            uint32_t len = read_uint32();

            PackedByteArray data;
            data.resize(len);
            byte_buffer.read_bytes(data.ptrw(), len);

            return String::utf8((const char*)data.ptr(), len);
        }

        void write_string(const String& value) {
            PackedByteArray utf8 = value.to_utf8_buffer();
            uint32_t len = utf8.size();

            write_uint32(len);

            byte_buffer.write_bytes(utf8.ptr(), len);
        }

        Vector2 read_vector2f() {
            engine::Vector2f result;
            byte_buffer.read<engine::Vector2f>(result);
            return Vector2(result.x, result.y);
        }

        void write_vector2i(Vector2i value) {
            engine::Vector2i typed_value = { value.x, value.y };
            byte_buffer.write<engine::Vector2i>(typed_value);
        }

        Vector2i read_vector2i() {
            engine::Vector2i result;
            byte_buffer.read<engine::Vector2i>(result);
            return Vector2i(result.x, result.y);
        }

        void write_buffer(GodotByteBuffer* buffer) {
            byte_buffer.write<engine::ByteBuffer>(buffer->byte_buffer);
        }

        GodotByteBuffer* read_buffer() {
            GodotByteBuffer* buffer = allocate_godot_byte_buffer();
            byte_buffer.read<engine::ByteBuffer>(buffer->byte_buffer);
            return buffer;
        }

        void initialise(int64_t size, int64_t offset, bool allow_resize) {
            if (size < 0) {
                engine::panic("size cannot be negative");
            }
            byte_buffer.initialise(size, offset, allow_resize or size == 0);
        }

        void reference_data(GodotByteBuffer* buffer) {
            byte_buffer.reference_data(buffer->byte_buffer);
        }

        GodotByteBuffer* dereference_data(int64_t size, int64_t offset) {
            GodotByteBuffer* buffer = allocate_godot_byte_buffer();
            byte_buffer.dereference_data(buffer->byte_buffer, size, offset);
            return buffer;
        }

        GodotByteBuffer* copy() const {
            GodotByteBuffer* created = allocate_godot_byte_buffer();
            created->copy_from(this);
            return created;
        }

        void copy_from(const GodotByteBuffer* other) {
            byte_buffer.copy_from(other->byte_buffer);
        }

        void shallow_copy_from(const GodotByteBuffer* other) {
            byte_buffer.shallow_copy_from(other->byte_buffer);
        }

        void copy_from_bytes(GodotByteBuffer* other, int64_t chunk_size) {
            if (chunk_size < 0) {
                engine::panic("size cannot be negative");
            }
            byte_buffer.copy_from_bytes(other->byte_buffer, chunk_size);
        }

        void resize(int64_t new_size) {
            if (new_size < 0) {
                engine::panic("size cannot be negative");
            }
            byte_buffer.resize(new_size);
        }

        int64_t get_size() const {
            return byte_buffer.size;
        }

        void set_allow_resize(bool new_allow_resize) {
            byte_buffer.allow_resize = new_allow_resize;
        }

        bool get_allow_resize() const {
            return byte_buffer.allow_resize;
        }

        void set_offset(int64_t new_offset) {
            if (new_offset < 0) {
                engine::panic("offset cannot be negative");
            }
            byte_buffer.offset = new_offset;
        }

        int64_t get_offset() const {
            return byte_buffer.offset;
        }

        bool get_owns_data() const {
            return byte_buffer.owns_data;
        }

        bool is_valid() const {
            return byte_buffer.is_valid();
        }

        void clear() {
            byte_buffer.clear();
        }

        void print_out() {
            print(byte_buffer);
        }

        void deallocate() {
            free_godot_byte_buffer(this);
        }

        static std::vector<GodotByteBuffer*> godot_byte_buffer_pool;

        static GodotByteBuffer* allocate_godot_byte_buffer() {
            if (godot_byte_buffer_pool.size() > 0) {
                GodotByteBuffer* allocated = godot_byte_buffer_pool.back();
                godot_byte_buffer_pool.pop_back();
                allocated->clear();
                return allocated;
            }
            else {
                return memnew(GodotByteBuffer);
            }
        }

        static void free_godot_byte_buffer(GodotByteBuffer* byte_buffer) {
            godot_byte_buffer_pool.push_back(byte_buffer);
        }
    };

    std::vector<GodotByteBuffer*> GodotByteBuffer::godot_byte_buffer_pool;
}

namespace engine {
    namespace bullshit {
        godot::GodotByteBuffer* allocate_godot_byte_buffer() {
            return godot::GodotByteBuffer::allocate_godot_byte_buffer();
        }

        void free_godot_byte_buffer(godot::GodotByteBuffer* byte_buffer) {
            godot::GodotByteBuffer::free_godot_byte_buffer(byte_buffer);
        }
    }
}