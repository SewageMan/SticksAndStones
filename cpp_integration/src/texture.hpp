#pragma once

#include <helper.hpp>

namespace engine {

	constexpr size_t FLOOR = 0;
	constexpr size_t BLOCK_BELOW = 1;
	constexpr size_t PLAYER = 2;
	constexpr size_t BLOCK_ABOVE = 3;
	constexpr size_t ROOF = 4;
	constexpr size_t BASIC_OVERLAY = 5;

	constexpr size_t AMOUNT_DRAW_LAYERS = 6;

	typedef uint32_t TextureId;
	typedef uint32_t DrawElementId;

	namespace bullshit {
		TextureId get_texture_id(std::string texture_path);
		DrawElementId get_multimesh_id(uint32_t layer_id, uint32_t sublayer_id, TextureId texture_id);
		DrawElementId add_to_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size);
		void edit_in_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size);
		void delete_from_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id);
	}

	struct Multimesh {
		DrawElementId layer_id;
		DrawElementId sublayer_id;
		DrawElementId multimesh_id;

		Multimesh() {};

		Multimesh(DrawElementId layer_id, DrawElementId sublayer_id, DrawElementId multimesh_id) : layer_id(layer_id), sublayer_id(sublayer_id), multimesh_id(multimesh_id) {}

		DrawElementId add_element(Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) {
			return bullshit::add_to_multimesh(layer_id, sublayer_id, multimesh_id, pos_chunks, pos_units, size);
		}

		void edit_element(DrawElementId element_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) {
			bullshit::edit_in_multimesh(layer_id, sublayer_id, multimesh_id, element_id, pos_chunks, pos_units, size);
		}

		void delete_element(DrawElementId element_id) {
			bullshit::delete_from_multimesh(layer_id, sublayer_id, multimesh_id, element_id);
		}
	};

	struct MultimeshElement {
		DrawElementId layer_id;
		DrawElementId sublayer_id;
		DrawElementId multimesh_id;
		DrawElementId element_id;

		MultimeshElement() {};

		MultimeshElement(DrawElementId layer_id, DrawElementId sublayer_id, TextureId texture_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) : layer_id(layer_id), sublayer_id(sublayer_id) {
			multimesh_id = bullshit::get_multimesh_id(layer_id, sublayer_id, texture_id);
			element_id = bullshit::add_to_multimesh(layer_id, sublayer_id, multimesh_id, pos_chunks, pos_units, size);
			print(multimesh_id, "multimesh_id");
			print(element_id, "element_id");
		}

		void edit_element(Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) {
			bullshit::edit_in_multimesh(layer_id, sublayer_id, multimesh_id, element_id, pos_chunks, pos_units, size);
		}

		~MultimeshElement() {
			bullshit::delete_from_multimesh(layer_id, sublayer_id, multimesh_id, element_id);
		}
	};

	struct Texture {
		TextureId texture_id;

		Texture() {};

		Texture(TextureId texture_id) : texture_id(texture_id) {};

		Texture(std::string texture_path) {
			texture_id = bullshit::get_texture_id(texture_path);
		}

		Multimesh get_multimesh(uint32_t layer_id, uint32_t sublayer_id) {
			DrawElementId multimesh_id = bullshit::get_multimesh_id(layer_id, sublayer_id, texture_id);
			print(multimesh_id, "multimesh_id");
			return Multimesh(layer_id, sublayer_id, multimesh_id);
		}
	};
}