#pragma once

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/quad_mesh.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/canvas_item_material.hpp>
#include <godot_cpp/classes/quad_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/main_loop.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/multi_mesh_instance2d.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera2d.hpp>

#include <helper.hpp>

namespace engine {

    godot::String to_gdstring(std::string input) {
        return godot::String::utf8(input.c_str());
    }

    void godot_quit() {
        godot::MainLoop* main_loop = godot::Engine::get_singleton()->get_main_loop();
        godot::SceneTree* scene_tree = godot::Object::cast_to<godot::SceneTree>(main_loop);
        scene_tree->quit();
    }

    struct GodotStreamBuffer : public std::streambuf {

        std::string buffer;

        std::streamsize xsputn(const char* s, std::streamsize n) override {
            buffer.append(s, n);
            size_t pos = buffer.find_last_of('\n');
            if (pos != std::string::npos) {
                flush();
            }
            return n;
        }

        int overflow(int c) override {
            if (c != EOF) {
                buffer.push_back(static_cast<char>(c));
                if (c == '\n') {
                    flush();
                }
            }
            return c;
        }

        int sync() override {
            if (!buffer.empty()) {
                flush();
            }
            return 0;
        }

        void flush() {
            godot::String gd_str(buffer.c_str());
            godot::UtilityFunctions::print(gd_str);
            buffer.clear();
        }
    };
}