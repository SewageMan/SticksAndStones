#pragma once

#include <godot_helper.hpp>

namespace godot {

    struct GodotByteBuffer : Object {
        GDCLASS(GodotByteBuffer, Object)

    public:
        engine::ByteBuffer byte_buffer;

        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("read_uint32"), &GodotByteBuffer::read_uint32);
            ClassDB::bind_method(D_METHOD("write_uint32", "value"), &GodotByteBuffer::write_uint32);
            ClassDB::bind_method(D_METHOD("initialise", "size"), &GodotByteBuffer::initialise);
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

        int64_t read_uint32() {
            uint32_t result;
            byte_buffer.read<uint32_t>(result);
            return result;
        }

        void write_uint32(int64_t value) {
            uint32_t typed_value = value;
            byte_buffer.write<uint32_t>(typed_value);
        }

        void initialise(int64_t size) {
            if (size < 0) {
                engine::panic("size cannot be negative");
            }
            engine::print(size);
            byte_buffer.initialise(size);
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