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

		LoadZone(bool run_graphics, Dimension* dimension, std::vector<Vector2p>& running_chunks_pos) : run_graphics(run_graphics), dimension(dimension) {
			set_running_chunks(running_chunks_pos);
		}

		void set_running_chunks(std::vector<Vector2p>& new_running_chunks_pos) {
			if (run_graphics) {
				set_running_chunks_internal<true>(new_running_chunks_pos);
			}
			else {
				set_running_chunks_internal<false>(new_running_chunks_pos);
			}
		}

		template <bool run_graphics>
		void set_running_chunks_internal(std::vector<Vector2p>& new_running_chunks_pos) {

			std::vector<Chunk*> new_running_chunks;
			for (Vector2p chunk_pos : new_running_chunks_pos) {
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
}