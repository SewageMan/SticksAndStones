#pragma once

#include <integration_core.hpp>

using namespace godot;

struct CppGodotAdapter : public Object {
    GDCLASS(CppGodotAdapter, Object)
        
    
    bool valid_state = true;
    uint32_t left_frames = 0;


    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("initialise", "floor_layer_node", "objects_layer_below_node", "objects_layer_above_node", "roof_layer_node"), &CppGodotAdapter::initialise);
        ClassDB::bind_method(D_METHOD("process", "delta"), &CppGodotAdapter::process);
        ClassDB::bind_method(D_METHOD("is_valid_state"), &CppGodotAdapter::is_valid_state);
    }

    bool is_valid_state() {
        return valid_state;
    }

    void start_exit() {
        valid_state = false;
        left_frames = 2;
    }

    void on_invalid_check() {
        if (left_frames == 0) {
            engine::godot_quit();
        }
        else {
            left_frames -= 1;
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

    void initialise(Node2D* floor_layer_node, Node2D* objects_layer_below_node, Node2D* objects_layer_above_node, Node2D* roof_layer_node) {
        
        if (not valid_state) {
            on_invalid_check();
            return;
        }
        
        try {
            engine::IntegrationCore::instance.initialise(floor_layer_node, objects_layer_below_node, objects_layer_above_node, roof_layer_node);
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
            engine::IntegrationCore::instance.process(static_cast<double>(delta));
            //engine::print(engine::GraphicsManager::instance.get_texture_id("imaginary path not existing anywhere"));
        }
        catch (const engine::CriticalExceptionStopGodot exception) {
            on_exception_print(exception, "Critical Error on process");
            start_exit();
        }
    }
};