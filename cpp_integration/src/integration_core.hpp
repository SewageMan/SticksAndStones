#pragma once

#include <godot_helper.hpp>
#include <main_engine.hpp>
#include <graphics_manager.hpp>
#include <texture.hpp>
#include <event_bus_manager.hpp>

class CppCore;

namespace engine {
    struct TestEventBus : EventBus {
        TestEventBus() : EventBus(0) {

        }

        virtual bool receive(EventChannelId channel_id, EventPackage payload) {
            print("received integer in c++:", payload.uint32);
            if (payload.uint32 > 0) {
                payload.uint32 -= 1;
                return send(1, payload);
            }
            return true;
        }

    };

	struct IntegrationCore {

        Texture texture;
        Multimesh multimesh;
        DrawElementId element_id;

        Texture texture2;
        Multimesh multimesh2;
        DrawElementId element_id2;

        TestEventBus* test_event_bus;

        Seconds passed = 0;

        void initialise(godot::Node2D* floor_layer_node, godot::Node2D* objects_layer_below_node, godot::Node2D* player_layer_node, godot::Node2D* objects_layer_above_node, godot::Node2D* roof_layer_node, godot::Node2D* player_basic_overlay, godot::Camera2D* world_camera) {
            engine::out_stream = new std::ostream(new GodotStreamBuffer());
            engine::print("asfassdf", engine::Vector2<int>(515, 56432));

            GraphicsManager::instance.initialise(world_camera, floor_layer_node, objects_layer_below_node, player_layer_node, objects_layer_above_node, roof_layer_node, player_basic_overlay);
            EventBusManager::instance.initialise();
            MainEngine::instance.initialise();

            test_event_bus = new TestEventBus();

            ByteBuffer buffer(10);

            uint32_t value = 10;
            print(value, "before writing");
            buffer.write<uint32_t>(value);
            print(value, "after writing");
            buffer.offset = 0;
            uint32_t result = 999;
            print(result, "before reading");
            buffer.read<uint32_t>(result);
            print(result,"after reading");

            texture = Texture("res://Sprites/Entities/Storage/chest.png");

            multimesh = texture.get_multimesh(BASIC_OVERLAY, 1);

            DrawElementId thing1 = multimesh.add_element({0, 0}, { 350, 50 }, { 100,100 });

            element_id = multimesh.add_element({ 0, 0 }, {0,0 }, {100,100});

            texture2 = Texture("res://Sprites/Entities/Storage/chest_inventory.png");

            multimesh2 = texture2.get_multimesh(BASIC_OVERLAY, 0);

            element_id2 = multimesh2.add_element({ 0, 0 }, { 0,0 }, { 100,100 });

            DrawElementId thing2 = multimesh2.add_element({ 0, 0 }, { 400,50 }, { 100,100 });

            //multimesh.delete_element(thing1);

            //multimesh2.delete_element(thing2);
        }

        void process(Seconds delta) {
            MainEngine::instance.perform_process(delta);
            passed += delta;

            multimesh.edit_element(element_id, { 0, 0 }, Vector2f(std::floor(passed * 10.0f + 0.0f), std::floor(passed * 10.0f + 0.0f) ), { 90,90 });

            multimesh2.edit_element(element_id2, { 0, 0 }, Vector2f( passed * 10.0f + 100.0f, passed * 10.0f + 100.0f ), { 100,100 });

            //camera->set_position(godot::Vector2(passed * 10.0f, passed * 10.0f));

            //camera->set_zoom(godot::Vector2(1.0f / (passed * 0.1f + 1.0f), 1.0f / (passed * 0.1f + 1.0f)));

            auto res = to_local_coords({ passed * chunk_size_units / 2.0f,0.0f });

            GraphicsManager::instance.set_camera_pos(res.first, res.second);

            GraphicsManager::instance.set_camera_zoom(0.1f);

            //GraphicsManager::instance.set_camera_zoom(1.0f / (passed * 0.1f + 1.0f));
        }

        static IntegrationCore instance;
	};

    IntegrationCore IntegrationCore::instance;
}