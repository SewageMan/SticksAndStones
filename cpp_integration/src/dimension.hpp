#pragma once

#include <load_zone.hpp>

namespace engine {

	struct Dimension {
		std::string name;

		std::vector<Chunk*> loaded_chunks;

		std::vector<Chunk*> process_running_chunks;
		std::vector<Chunk*> graphic_running_chunks;

		TileDescriptor* default_ground;

		Dimension(std::string name, TileDescriptor* default_ground) : name(name), default_ground(default_ground) {}

		virtual void perform_process(Seconds delta) {
			for (Chunk* chunk : process_running_chunks) {
				chunk->perform_block_process(delta);
			}
			for (Chunk* chunk : graphic_running_chunks) {
				chunk->perform_block_process(delta);
			}
		}

		Chunk* get_chunk(Vector2p pos_chunks) {
			for (Chunk* chunk : loaded_chunks) {
				if (chunk->pos_chunks == pos_chunks) {
					return chunk;
				}
			}
			return nullptr;
		}

		Chunk* get_load_chunk(Vector2p pos_chunks) {
			for (Chunk* chunk : loaded_chunks) {
				if (chunk->pos_chunks == pos_chunks) {
					return chunk;
				}
			}
			return load_chunk(pos_chunks);
		}

		Chunk* load_chunk(Vector2p pos_chunks) {
			Chunk* chunk = new Chunk(this, pos_chunks);
			loaded_chunks.push_back(chunk);
			bool read_from_file = try_read_chunk(chunk);
			if (not read_from_file) {
				generate_chunk(chunk);
			}
			return chunk;
		}

		bool try_read_chunk(Chunk* chunk) {
			return false;
		}

		virtual void generate_chunk(Chunk* chunk) {
			chunk->set_default_ground();
			chunk->set_no_blocks();
		}

		void start_process(Chunk* chunk) {
			if (not chunk->data_initialised) {
				chunk->initialise_data();
			}
			chunk->enable_data_process();
			process_running_chunks.push_back(chunk);
		}

		void start_graphics(Chunk* chunk) {
			if (not chunk->graphics_initialised) {
				chunk->initialise_graphics();
			}
			chunk->enable_graphics();
			graphic_running_chunks.push_back(chunk);
		}

		void end_process(Chunk* chunk) {
			chunk->disable_data_process();
			std::erase(process_running_chunks, chunk);
		}

		void end_graphics(Chunk* chunk) {
			chunk->disable_graphics();
			std::erase(graphic_running_chunks, chunk);
		}
	};

	namespace bullshit {
		Chunk* get_load_chunk(Dimension* dimension, Vector2p pos_chunks) {
			return dimension->get_load_chunk(pos_chunks);
		}

		TileDescriptor* get_default_ground(Dimension* dimension) {
			return dimension->default_ground;
		}

		void start_process(Dimension* dimension, Chunk* chunk) {
			dimension->start_process(chunk);
		}

		void start_graphics(Dimension* dimension, Chunk* chunk) {
			dimension->start_graphics(chunk);
		}

		void end_process(Dimension* dimension, Chunk* chunk) {
			dimension->end_process(chunk);
		}

		void end_graphics(Dimension* dimension, Chunk* chunk) {
			dimension->end_graphics(chunk);
		}
	}
}