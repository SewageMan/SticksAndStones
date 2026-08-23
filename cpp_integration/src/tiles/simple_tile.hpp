#pragma once

#include <world.hpp>


namespace engine {

	struct SimpleTile;

	struct SimpleTileDescriptor : public TileDescriptor {

		Texture texture;
		Multimesh multimesh;

		SimpleTileDescriptor(std::string descriptor_name, Texture texture) : TileDescriptor(descriptor_name), texture(texture) {
			multimesh = texture.get_multimesh(FLOOR, 0);
		}

		virtual Tile* make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) override;
	};

	struct SimpleTile : public Tile {
		using Tile::Tile;
		NARROW_DESCRIPTOR(SimpleTileDescriptor)

		DrawElementId element_id;

		virtual void enable_graphics() override {
			element_id = descriptor()->multimesh.add_element(pos_units(),block_size_vec);
		}

		virtual void disable_graphics() override {
			descriptor()->multimesh.delete_element(element_id);
		}
	};

	Tile* SimpleTileDescriptor::make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) {
		return new SimpleTile(linked_chunk, this, pos_blocks);
	}
}