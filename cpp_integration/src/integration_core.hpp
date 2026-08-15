#pragma once

#include <godot_helper.hpp>
#include <main_engine.hpp>
#include <graphics_manager.hpp>
#include <texture.hpp>

class CppCore;

namespace engine {
	struct IntegrationCore {

        Texture texture;
        Multimesh multimesh;
        DrawElementId element_id;

        Texture texture2;
        Multimesh multimesh2;
        DrawElementId element_id2;

        Seconds passed = 0;

        void initialise(godot::Node2D* floor_layer_node, godot::Node2D* objects_layer_below_node, godot::Node2D* objects_layer_above_node, godot::Node2D* roof_layer_node) {
            engine::out_stream = new std::ostream(new GodotStreamBuffer());
            engine::print("asfassdf", engine::Vector2<int>(515, 56432));
            GraphicsManager::instance.initialise(floor_layer_node, objects_layer_below_node, objects_layer_above_node, roof_layer_node);
            MainEngine::instance.initialise();

            texture = Texture("res://Sprites/Entities/Storage/chest.png");

            multimesh = texture.get_multimesh(0, 1);

            DrawElementId thing1 = multimesh.add_element<int,int>({ 350,50 }, { 100,100 });

            element_id = multimesh.add_element<int, int>({0,0 }, {100,100});

            texture2 = Texture("res://Sprites/Entities/Storage/chest_inventory.png");

            multimesh2 = texture2.get_multimesh(0, 0);

            element_id2 = multimesh2.add_element<int, int>({ 0,0 }, { 100,100 });

            DrawElementId thing2 = multimesh2.add_element<int, int>({ 400,50 }, { 100,100 });

            //multimesh.delete_element(thing1);

            //multimesh2.delete_element(thing2);
        }

        void process(Seconds delta) {
            MainEngine::instance.perform_process(delta);
            passed += delta;

            multimesh.edit_element<double, int>(element_id, {std::floor(passed * 10.0f + 0.0f), std::floor(passed * 10.0f + 0.0f) }, { 90,90 });

            multimesh2.edit_element<double, int>(element_id2, { passed * 10.0f + 100.0f, passed * 10.0f + 100.0f }, { 100,100 });

        }

        static IntegrationCore instance;
	};

    IntegrationCore IntegrationCore::instance;
}