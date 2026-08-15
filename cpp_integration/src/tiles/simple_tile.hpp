#pragma once

#include <tiles/base_tile.hpp>


namespace engine {

	namespace bullshit {
		DrawElementId simple_tile_descriptor_add_element(WorldObject* linked_object, Vector2Units pos);
	}

	struct SimpleTileDescriptor;

	struct SimpleTileRender : public TileRender {
		using TileRender::TileRender;
		DrawElementId element_id;

		SimpleTileRender(WorldObject* linked_object) : TileRender(linked_object) {
			element_id = bullshit::simple_tile_descriptor_add_element(linked_object, linked_object->pos_units());
		}
	};

	struct SimpleTile : public Tile {
		using Tile::Tile;

		virtual SimpleTileRender* initialise_render() override {
			return new SimpleTileRender(this);
		}
	};

	struct SimpleTileDescriptor : public TileDescriptor {
		
		Texture texture;
		Multimesh multimesh;

		SimpleTileDescriptor(std::string descriptor_name, Texture texture) : TileDescriptor(descriptor_name), texture(texture) {
			multimesh = texture.get_multimesh(FLOOR, 0);
		}

		virtual SimpleTile* make_object(Chunk* linked_chunk, Vector2Block pos_blocks) override {
			return new SimpleTile(linked_chunk, this, pos_blocks);
		}
	};

	namespace bullshit {
		DrawElementId simple_tile_descriptor_add_element(WorldObject* linked_object, Vector2Units pos) {
			SimpleTileDescriptor* descriptor = static_cast<SimpleTileDescriptor*>(linked_object->descriptor);
			return descriptor->multimesh.add_element(pos, block_size_vec);
		};
	}
}