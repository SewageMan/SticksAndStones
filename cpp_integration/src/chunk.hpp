#pragma once

#include <tiles/tile_descriptor.hpp>

namespace engine {

	struct Chunk;

	struct Dimension;

	namespace bullshit {
		Chunk* get_load_chunk(Dimension* dimension, Vector2p pos_chunks);

		TileDescriptor* get_default_ground(Dimension* dimension);
	}

	struct Chunk {
		Dimension* dimension;

		Vector2p pos_chunks;
		Vector2p pos_blocks;
		Vector2p pos_units;

		bool data_initialised = false;
		bool graphics_initialised = false;
		bool process_running = false;
		bool graphics_running = false;

		RefCounter loading_process = 0;
		RefCounter loading_graphics = 0;

		std::array<Chunk*, 4> neighbours = { nullptr, nullptr, nullptr, nullptr };

		ChunkMatrix<std::vector<TileDescriptor*>> floor_stacks;

		Chunk(Dimension* dimension, Vector2p pos_chunks) : dimension(dimension), pos_chunks(pos_chunks), pos_blocks(pos_chunks* chunk_size_blocks), pos_units(pos_chunks* chunk_size_units) {}

		void initialise_data() {
			neighbours[TOP] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(0, 1));
			neighbours[RIGHT] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(1, 0));
			neighbours[BOTTOM] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(0, -1));
			neighbours[LEFT] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(-1, 0));
		}

		void initialise_graphics() {

		}

		void perform_process(Seconds delta) {

		}

		void perform_graphics_process(Seconds delta) {

		}

		void set_default_ground() {
			TileDescriptor* default_ground = bullshit::get_default_ground(dimension);
			for (size_t x = 0; x < chunk_size_blocks; ++x) {
				for (size_t y = 0; y < chunk_size_blocks; ++y) {
					floor_stacks[x][y].resize(1);
					floor_stacks[x][y][0] = default_ground;
				}
			}
		}
	};
}