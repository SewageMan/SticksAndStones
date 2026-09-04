#pragma once

#include <integration_core.hpp>
#include <godot_byte_buffer.hpp>

using namespace godot;

#define BIND_EVENT_BUS_CONSTANT(name) \
    ClassDB::bind_integer_constant(get_class_static(), "", #name, engine::event_bus_ids::event_bus_ids::name)

struct CppGodotAdapter : public Object {
    GDCLASS(CppGodotAdapter, Object)
public:
    
    bool valid_state = true;
    uint32_t left_frames = 0;

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("initialise", "floor_layer_node", "objects_layer_below_node", "player_layer_node", "objects_layer_above_node", "roof_layer_node", "player_basic_overlay", "world_camera"), &CppGodotAdapter::initialise);
        ClassDB::bind_method(D_METHOD("process", "delta"), &CppGodotAdapter::process);
        ClassDB::bind_method(D_METHOD("is_valid_state"), &CppGodotAdapter::is_valid_state);
        ClassDB::bind_method(D_METHOD("subscribe_event_bus", "channel_id", "event_bus"), &CppGodotAdapter::subscribe_event_bus);
        ClassDB::bind_method(D_METHOD("unsubscribe_event_bus", "channel_id", "event_bus_id"), &CppGodotAdapter::unsubscribe_event_bus);
        ClassDB::bind_method(D_METHOD("send", "channel_id", "payload_buffer"), &CppGodotAdapter::send);
        ClassDB::bind_method(D_METHOD("get_payload_size"), &CppGodotAdapter::get_payload_size_godot);
        ClassDB::bind_method(D_METHOD("debug_get_id"), &CppGodotAdapter::debug_get_id);

        BIND_EVENT_BUS_CONSTANT(RESERVED_ZERO_INDEX);
        BIND_EVENT_BUS_CONSTANT(SET_PLAYER_STATE);
        BIND_EVENT_BUS_CONSTANT(SET_PLAYER_SPEED); 
        BIND_EVENT_BUS_CONSTANT(UPDATE_CAMERA_CHUNK);
        BIND_EVENT_BUS_CONSTANT(MULTIPLY_CAMERA_ZOOM);
    }

    int64_t debug_get_id() {
        return 0; // return engine::IntegrationCore::instance.test_event_bus->bus_id;
    }

    engine::EventBusId subscribe_event_bus(int64_t channel_id, Object* event_bus) {
        if (not valid_state) {
            on_invalid_check();
            return 0;
        }

        try {
            return engine::EventBusManager::instance.subscribe_godot(channel_id, event_bus);
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on inilialise");
            start_exit();
        }
    }
    void unsubscribe_event_bus(int64_t channel_id, int64_t event_bus_id) {

        if (not valid_state) {
            on_invalid_check();
            return;
        }

        try {
            engine::EventBusManager::instance.unsubscribe_godot(channel_id, event_bus_id);
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on inilialise");
            start_exit();
        }
    }
    bool send(int64_t channel_id, GodotByteBuffer* payload_buffer) {

        if (not valid_state) {
            on_invalid_check();
            return false;
        }

        try {
            engine::ByteBuffer& byte_buffer = payload_buffer->byte_buffer;
            if (byte_buffer.size != get_payload_size() or not byte_buffer.is_valid()) {
                if (not byte_buffer.is_valid()) {
                    engine::panic("passing invalid buffer");
                }
                else {
                    engine::panic("passing byte buffer with incorrect size, buffer size: " + std::to_string(byte_buffer.size) + ", expected size: " + std::to_string(get_payload_size()));
                }
            }
            return engine::EventBusManager::instance.send(channel_id, std::bit_cast<engine::EventPackage>(*reinterpret_cast<uint8_t(*)[sizeof(engine::EventPackage)]>(byte_buffer.data)));
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on inilialise");
            start_exit();
        }
    }

    static constexpr int64_t get_payload_size() {
        return sizeof(engine::EventPackage);
    }

    constexpr int64_t get_payload_size_godot() const {
        return sizeof(engine::EventPackage);
    }

    bool is_valid_state() const {
        return valid_state;
    }

    void start_exit() {
        valid_state = false;
        left_frames = 3;
    }

    void on_invalid_check() {
        if (left_frames == 0) {
            engine::godot_quit();
        }
        else {
            left_frames -= 1;
            engine::print("frames until shutting down:", left_frames);
        }
    }

    void on_exception_print(const engine::CriticalExceptionStopGodot exception, std::string header) {
        engine::print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        engine::print(header);
        engine::print(exception.what());
        engine::print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        engine::print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        engine::print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        godot::OS::get_singleton()->alert(engine::to_gdstring(exception.what()), engine::to_gdstring(header));
        OS::get_singleton()->delay_msec(100);
    }

    void initialise(Node2D* floor_layer_node, Node2D* objects_layer_below_node, godot::Node2D* player_layer_node, Node2D* objects_layer_above_node, Node2D* roof_layer_node, godot::Node2D* player_basic_overlay, godot::Camera2D* world_camera) {
        
        if (not valid_state) {
            on_invalid_check();
            return;
        }
        
        try {
            engine::IntegrationCore::instance.initialise(floor_layer_node, objects_layer_below_node, player_layer_node, objects_layer_above_node, roof_layer_node, player_basic_overlay, world_camera);
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on inilialise");
            start_exit();
        }
    }

    void process(float delta) {

        if (not valid_state) {
            on_invalid_check();
            return;
        }

        try {
            engine::IntegrationCore::instance.process(engine::Time::seconds(delta));
            //engine::print(engine::GraphicsManager::instance.get_texture_id("imaginary path not existing anywhere"));
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on process");
            start_exit();
        }
    }
};