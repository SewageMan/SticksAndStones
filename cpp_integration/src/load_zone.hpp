#pragma once

#include <chunk.hpp>

namespace engine {

	namespace bullshit {
		void start_process(Dimension* dimension, Chunk* chunk);

		void start_graphics(Dimension* dimension, Chunk* chunk);

		void end_process(Dimension* dimension, Chunk* chunk);

		void end_graphics(Dimension* dimension, Chunk* chunk);
	}

	struct LoadZone {
		Dimension* dimension;
		std::vector<Chunk*> running_chunks;
		bool run_graphics;
		
		LoadZone(bool run_graphics, Dimension* dimension) : run_graphics(run_graphics), dimension(dimension) {}

		LoadZone(bool run_graphics, Dimension* dimension, std::span<Vector2Chunk> running_chunks_pos) : run_graphics(run_graphics), dimension(dimension) {
			set_running_chunks(running_chunks_pos);
		}

		void set_running_chunks(std::span<Vector2Chunk> new_running_chunks_pos) {
			if (run_graphics) {
				set_running_chunks_internal<true>(new_running_chunks_pos);
			}
			else {
				set_running_chunks_internal<false>(new_running_chunks_pos);
			}
		}

		template <bool run_graphics>
		void set_running_chunks_internal(std::span<Vector2Chunk> new_running_chunks_pos) {

			std::vector<Chunk*> new_running_chunks;
			for (Vector2Chunk chunk_pos : new_running_chunks_pos) {
				new_running_chunks.push_back(bullshit::get_load_chunk(dimension, chunk_pos));
			}

			for (Chunk* chunk : running_chunks) {
				if (not can_find_in_vec(new_running_chunks, chunk)) {
					if constexpr (run_graphics) {
						if (--(chunk->loading_graphics) == 0) {
							bullshit::end_graphics(dimension, chunk);
						}
					}
					if (--(chunk->loading_process) == 0) {
						bullshit::end_process(dimension, chunk);
					}
				}
			}
			
			for (Chunk* chunk : new_running_chunks) {
				if (not can_find_in_vec(running_chunks, chunk)) {
					if (++(chunk->loading_process) == 1) {
						bullshit::start_process(dimension, chunk);
					}
					if constexpr (run_graphics) {
						if (++(chunk->loading_graphics) == 1) {
							bullshit::start_graphics(dimension, chunk);
						}
					}
				}
			}
			running_chunks = new_running_chunks;
		}

		void clear() {
			set_running_chunks({});
		}

		~LoadZone() {
			for (Chunk* chunk : running_chunks) {
				if (run_graphics) {
					if (--(chunk->loading_graphics) == 0) {
						bullshit::end_graphics(dimension, chunk);
					}
				}
				if (--(chunk->loading_process) == 0) {
					bullshit::end_process(dimension, chunk);
				}
			}
		}
	};

	struct LoadZoneArea : LoadZone {

		LoadZoneArea(bool run_graphics, Dimension* dimension) : LoadZone(run_graphics, dimension) {
			
		}

		LoadZoneArea(bool run_graphics, Dimension* dimension, Vector2Chunk center, pos_t size) : LoadZone(run_graphics, dimension) {
			set(center, size);
		}

		void set(Vector2Chunk center, pos_t size) {
			std::vector<Vector2Chunk> new_chunks;
			pos_t one_side = (size * 2 + 1);
			new_chunks.reserve(one_side* one_side);
			size_t counter = 0;
			for (pos_t x = center.x - size; x <= center.x + size; ++x) {
				for (pos_t y = center.y - size; y <= center.y + size; ++y) {
					new_chunks.emplace_back(x, y);
					print(x, y, new_chunks.size());
				}
			}
			set_running_chunks(new_chunks);
		}
	};
}