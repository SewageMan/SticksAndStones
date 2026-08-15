#pragma once

#include <godot_helper.hpp>
#include <texture.hpp>

class CppCore;

namespace engine {

	struct DrawMultimesh {
		godot::RID multimesh;
		DrawElementId allocated_size;
		DrawElementId used_size;
		std::vector<DrawElementId> element_ids;
		std::vector<DrawElementId> buffer_ids;
		std::vector<DrawElementId> free_elements;
	};

	struct DrawSubLayer {
		godot::RID linked_canvas_item;
		std::vector<DrawMultimesh> multimeshes;
	};

	struct DrawLayer {
		godot::Node2D* main_node;
		std::vector<DrawSubLayer> sublayers;
	};

	struct TextureInSubLayer {
		DrawElementId multimesh_id;
		bool is_present = false;
	};

	struct TextureInLayer {
		std::vector<TextureInSubLayer> texture_in_sublayers;
	};

	struct TextureContainer {
		godot::Ref<godot::Texture2D> texture_object;
		godot::Ref<godot::CanvasItemMaterial> material;
		godot::Ref<godot::QuadMesh> quad_mesh;
		std::array<TextureInLayer, 4> texture_in_layers;
	};

	struct GraphicsManager {
		
		static constexpr size_t multimesh_starting_size = 16;

		godot::AABB global_aabb = godot::AABB(godot::Vector3(-1e7, -1e7, -1e7), godot::Vector3(1e7, 1e7, 1e7));

		godot::RenderingServer* rendering_server;
		godot::ResourceLoader* texture_loader;

		std::vector<TextureContainer> textures;
		std::unordered_map<std::string, TextureId> texture_names;

		std::array<DrawLayer,4> draw_layers;

		void initialise(godot::Node2D* floor_layer_node, godot::Node2D* objects_layer_below_node, godot::Node2D* objects_layer_above_node, godot::Node2D* roof_layer_node) {
			rendering_server = godot::RenderingServer::get_singleton();
			texture_loader = godot::ResourceLoader::get_singleton();

			draw_layers[FLOOR].main_node = floor_layer_node;
			draw_layers[BLOCK_BELOW].main_node = objects_layer_below_node;
			draw_layers[BLOCK_ABOVE].main_node = objects_layer_above_node;
			draw_layers[ROOF].main_node = roof_layer_node;
		}

		TextureId get_texture_id(std::string texture_path) {
			auto found = texture_names.find(texture_path);
			if (found!=texture_names.end()) {
				return found->second;
			}
			TextureId texture_id = textures.size();
			texture_names.emplace(texture_path, texture_id);
			textures.emplace_back();
			TextureContainer& texture = textures.back();
			texture.texture_object = texture_loader->load(to_gdstring(texture_path));
			if (not (texture.texture_object.is_valid())) {
				panic("could not load the texture by path: '" + texture_path + "'");
			}

			texture.material.instantiate();
			texture.material->set_light_mode(godot::CanvasItemMaterial::LIGHT_MODE_UNSHADED);

			texture.quad_mesh.instantiate();
			texture.quad_mesh->set_size(godot::Vector2(1.0, 1.0));
			texture.quad_mesh->set_material(texture.material);

			return texture_id;
		}

		DrawElementId get_multimesh_id(uint32_t layer_id, uint32_t sublayer_id, TextureId texture_id) {
			TextureContainer& texture = textures[texture_id];
			TextureInLayer& in_layer = texture.texture_in_layers[layer_id];
			if (sublayer_id >= in_layer.texture_in_sublayers.size()) {
				in_layer.texture_in_sublayers.resize(sublayer_id + 1);
			}
			TextureInSubLayer& in_sublayer = in_layer.texture_in_sublayers[sublayer_id];
			DrawLayer& draw_layer = draw_layers[layer_id];
			if (in_sublayer.is_present) {
				return in_sublayer.multimesh_id;
			}
			if (draw_layer.sublayers.size() <= sublayer_id) {
				draw_layer.sublayers.resize(sublayer_id + 1);
			}
			if (not (draw_layer.sublayers[sublayer_id].linked_canvas_item.is_valid())) {
				godot::RID canvas_item = rendering_server->canvas_item_create();
				rendering_server->canvas_item_set_parent(canvas_item, draw_layer.main_node->get_canvas_item());
				rendering_server->canvas_item_set_z_index(canvas_item, sublayer_id);
				draw_layer.sublayers[sublayer_id].linked_canvas_item = canvas_item;
			}
			DrawSubLayer& draw_sublayer = draw_layer.sublayers[sublayer_id];
			in_sublayer.is_present = true;
			in_sublayer.multimesh_id = draw_sublayer.multimeshes.size();
			draw_sublayer.multimeshes.emplace_back();
			DrawMultimesh& multimesh = draw_sublayer.multimeshes.back();
			multimesh.allocated_size = multimesh_starting_size;
			multimesh.used_size = 0;
			multimesh.multimesh = rendering_server->multimesh_create();

			rendering_server->multimesh_allocate_data(multimesh.multimesh, multimesh_starting_size, godot::RenderingServer::MULTIMESH_TRANSFORM_2D, false, false);
			rendering_server->multimesh_set_mesh(multimesh.multimesh, texture.quad_mesh->get_rid());
			rendering_server->multimesh_set_visible_instances(multimesh.multimesh, multimesh.used_size);
			rendering_server->multimesh_set_custom_aabb(multimesh.multimesh, global_aabb); // this is to ensure its visible anywhere and not hidden by frustum culling
			rendering_server->canvas_item_add_multimesh(draw_sublayer.linked_canvas_item, multimesh.multimesh, texture.texture_object->get_rid());

			return in_sublayer.multimesh_id;
		}

		DrawElementId add_to_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, Vector2f pos, Vector2f size) {
			DrawLayer& draw_layer = draw_layers[layer_id];
			DrawSubLayer& draw_sublayer = draw_layer.sublayers[sublayer_id];
			DrawMultimesh& multimesh = draw_sublayer.multimeshes[multimesh_id];
			DrawElementId element_id;
			if (multimesh.free_elements.size() > 0) {
				element_id = multimesh.free_elements.back();
				multimesh.free_elements.pop_back();
				multimesh.element_ids[multimesh.used_size] = element_id;
			}
			else {
				if (multimesh.used_size >= multimesh.allocated_size) {
					godot::PackedFloat32Array old_buffer = rendering_server->multimesh_get_buffer(multimesh.multimesh);
					int64_t new_size = old_buffer.size() * 2;
					int64_t sucsess = old_buffer.resize(new_size);
					if (sucsess != godot::OK) {
						panic("failed to resize buffer to size " + std::to_string(new_size) + ", error code: " + std::to_string(sucsess));
					}
					multimesh.allocated_size *= 2;

					rendering_server->multimesh_allocate_data(multimesh.multimesh, multimesh.allocated_size, godot::RenderingServer::MULTIMESH_TRANSFORM_2D, false, false);
					rendering_server->multimesh_set_buffer(multimesh.multimesh, old_buffer);
					rendering_server->multimesh_set_custom_aabb(multimesh.multimesh, global_aabb); // this is to ensure its visible anywhere and not hidden by frustum culling
				}
				element_id = multimesh.buffer_ids.size();
				multimesh.buffer_ids.emplace_back();
				multimesh.element_ids.push_back(element_id);
			}
			godot::Transform2D transform;
			Vector2f origin = pos + size / 2;
			transform.set_origin(godot::Vector2(origin.x + 0x1p-5f, origin.y + 0x1p-5f));
			transform.set_scale(godot::Size2(size.x + 0x1p-4, size.y + 0x1p-4));

			rendering_server->multimesh_instance_set_transform_2d(multimesh.multimesh, multimesh.used_size, transform);

			multimesh.buffer_ids[element_id] = multimesh.used_size;
			multimesh.used_size += 1;

			rendering_server->multimesh_set_visible_instances(multimesh.multimesh, multimesh.used_size);

			return element_id;
		}

		void edit_in_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id, Vector2f pos, Vector2f size) {
			DrawLayer& draw_layer = draw_layers[layer_id];
			DrawSubLayer& draw_sublayer = draw_layer.sublayers[sublayer_id];
			DrawMultimesh& multimesh = draw_sublayer.multimeshes[multimesh_id];

			DrawElementId buffer_id = multimesh.buffer_ids[element_id];

			godot::Transform2D transform;
			Vector2f origin = pos + size / 2;
			transform.set_origin(godot::Vector2(origin.x + 0x1p-5f, origin.y + 0x1p-5f));
			transform.set_scale(godot::Size2(size.x + 0x1p-4, size.y + 0x1p-4));

			rendering_server->multimesh_instance_set_transform_2d(multimesh.multimesh, buffer_id, transform);
		}

		void delete_from_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id) {
			DrawLayer& draw_layer = draw_layers[layer_id];
			DrawSubLayer& draw_sublayer = draw_layer.sublayers[sublayer_id];
			DrawMultimesh& multimesh = draw_sublayer.multimeshes[multimesh_id];

			DrawElementId buffer_id = multimesh.buffer_ids[element_id];

			DrawElementId last_buffer = multimesh.used_size - 1;

			if (buffer_id != last_buffer) {
				godot::Transform2D other_transform = rendering_server->multimesh_instance_get_transform_2d(multimesh.multimesh, last_buffer);
				rendering_server->multimesh_instance_set_transform_2d(multimesh.multimesh, buffer_id, other_transform);
				DrawElementId last_element = multimesh.element_ids[last_buffer];
				multimesh.buffer_ids[last_element] = buffer_id;
				multimesh.element_ids[buffer_id] = last_element;
			}
			
			multimesh.free_elements.push_back(element_id);
			multimesh.used_size -= 1;

			rendering_server->multimesh_set_visible_instances(multimesh.multimesh, multimesh.used_size);
		}

		static GraphicsManager instance;
	};
	GraphicsManager GraphicsManager::instance;

	namespace bullshit {
		TextureId get_texture_id(std::string texture_path) {
			return GraphicsManager::instance.get_texture_id(texture_path);
		};
		DrawElementId get_multimesh_id(uint32_t layer_id, uint32_t sublayer_id, TextureId texture_id) {
			return GraphicsManager::instance.get_multimesh_id(layer_id, sublayer_id, texture_id);
		};
		DrawElementId add_to_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, Vector2f pos, Vector2f size) {
			return GraphicsManager::instance.add_to_multimesh(layer_id, sublayer_id, multimesh_id, pos, size);
		};
		void edit_in_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id, Vector2f pos, Vector2f size) {
			GraphicsManager::instance.edit_in_multimesh(layer_id, sublayer_id, multimesh_id, element_id, pos, size);
		};
		void delete_from_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id) {
			GraphicsManager::instance.delete_from_multimesh(layer_id, sublayer_id, multimesh_id, element_id);
		};
	}
}