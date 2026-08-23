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

		virtual void enable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks) override {
			element_id = descriptor()->multimesh.add_element(linked_chunk->pos_chunks, static_cast<Vector2Unitsf>(pos_blocks * block_size), static_cast<Vector2Unitsf>(block_size_vec));
		}

		virtual void disable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks) override {
			descriptor()->multimesh.delete_element(element_id);
		}
	};

	Tile* SimpleTileDescriptor::make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) {
		return new SimpleTile(linked_chunk, pos_blocks, this);
	}
}