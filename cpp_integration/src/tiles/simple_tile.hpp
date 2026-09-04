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

		virtual void enable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) override;

		virtual void disable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) override;

		virtual Tile* make_object(Chunk* linked_chunk, Vector2Units pos_blocks) override;
	};

	struct SimpleTile : public Tile {
		using Tile::Tile;
		NARROW_DESCRIPTOR(SimpleTileDescriptor)

		DrawElementId element_id;
	};

	Tile* SimpleTileDescriptor::make_object(Chunk* linked_chunk, Vector2Units pos_blocks) {
		return new SimpleTile(linked_chunk, pos_blocks, this);
	}

	void SimpleTileDescriptor::enable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_units) {
		static_cast<SimpleTile*>(world_obejct)->element_id = multimesh.add_element(linked_chunk->pos_chunks, static_cast<Vector2Unitsf>(pos_units), { 1.0f,1.0f });
	}

	void SimpleTileDescriptor::disable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {
		multimesh.delete_element(static_cast<SimpleTile*>(world_obejct)->element_id);
	}
}