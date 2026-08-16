#pragma once

#include <base_world_object.hpp>


namespace engine {

	struct Tile : public WorldObject {
		using WorldObject::WorldObject;
	};

	struct TileDescriptor : public ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;

		virtual Tile* make_object(Chunk* linked_chunk, Vector2Block pos_blocks) override {
			return new Tile(linked_chunk, this, pos_blocks);
		}
	};
}