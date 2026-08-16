#pragma once

#include <tiles/base_tile.hpp>
#include <blocks/base_block.hpp>
#include <process.hpp>

namespace engine {

	struct Chunk;

	struct Dimension;

	namespace bullshit {
		Chunk* get_load_chunk(Dimension* dimension, Vector2p pos_chunks);

		TileDescriptor* get_default_ground(Dimension* dimension);
	}

	struct Chunk {
		Dimension* dimension;

		Vector2Chunk pos_chunks;
		Vector2Block pos_blocks;
		Vector2Units pos_units;

		bool data_initialised = false;
		bool graphics_initialised = false;
		bool process_running = false;
		bool graphics_running = false;

		RefCounter loading_process = 0;
		RefCounter loading_graphics = 0;

		PersistentContainer<Process*, BlockProcessId> block_processes;
		PersistentContainer<Process*, RenderProcessId> render_processes;

		std::array<Chunk*, 4> neighbours = { nullptr, nullptr, nullptr, nullptr };

		ChunkMatrix<Tile*> floor_tiles;
		ChunkMatrix<Block*> blocks;

		Chunk(Dimension* dimension, Vector2p pos_chunks) : dimension(dimension), pos_chunks(pos_chunks), pos_blocks(pos_chunks* chunk_size_blocks), pos_units(pos_chunks* chunk_size_units) {}

		void initialise_data() {
			neighbours[TOP] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(0, 1));
			neighbours[RIGHT] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(1, 0));
			neighbours[BOTTOM] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(0, -1));
			neighbours[LEFT] = bullshit::get_load_chunk(dimension, pos_chunks + Vector2p(-1, 0));

			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->initialise_data();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->initialise_data();
				}
			}
		}

		void initialise_graphics() {
			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->initialise_graphics();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->initialise_graphics();
				}
			}
		}

		void enable_data_process() {
			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->enable_data_process();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->enable_data_process();
				}
			}
		}

		void disable_data_process() {
			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->disable_data_process();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->disable_data_process();
				}
			}
		}

		void enable_graphics() {
			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->enable_graphics();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->enable_graphics();
				}
			}
		}

		void disable_graphics() {
			for (auto array : floor_tiles) {
				for (Tile* tile : array) {
					tile->disable_graphics();
				}
			}
			for (auto array : blocks) {
				for (Block* block : array) {
					if (block == nullptr) continue;
					block->disable_graphics();
				}
			}
		}

		void perform_block_process(Seconds delta) {
			for (Process* process : block_processes) {
				process->perform_process(delta);
			}
		}

		void perform_render_process(Seconds delta) {
			for (Process* process : render_processes) {
				process->perform_process(delta);
			}
		}

		void set_default_ground() {
			TileDescriptor* default_ground = bullshit::get_default_ground(dimension);
			for (pos_t x = 0; x < chunk_size_blocks; ++x) {
				for (pos_t y = 0; y < chunk_size_blocks; ++y) {
					floor_tiles[x][y] = default_ground->make_object(this, pos_blocks + Vector2Block(x, y));
				}
			}
		}

		void set_no_blocks() {
			for (pos_t x = 0; x < chunk_size_blocks; ++x) {
				for (pos_t y = 0; y < chunk_size_blocks; ++y) {
					blocks[x][y] = nullptr;
				}
			}
		}
	};

	namespace bullshit {
		BlockProcessId register_block_process(Chunk* chunk, Process* process) {
			return chunk->block_processes.add_element(process);
		}
		RenderProcessId register_render_process(Chunk* chunk, Process* process) {
			return chunk->block_processes.add_element(process);
		}
	}
}