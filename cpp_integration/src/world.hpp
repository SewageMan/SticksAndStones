#pragma once

#include <helper.hpp>
#include <process.hpp>
#include <texture.hpp>

namespace engine {

	typedef uint32_t ObjectDescriptorId;

	struct Chunk;
	struct Dimension;
	struct LoadZone;
	
	struct WorldObject;
	struct ObjectDescriptor;

	struct Tile;
	struct TileDescriptor;

	struct Block;
	struct BlockDescriptor;

	namespace bullshit {
		void register_descriptor(ObjectDescriptor* descriptor);
	}

	struct Chunk {
		Dimension* dimension;

		Vector2Chunks pos_chunks;

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

		Chunk(Dimension* dimension, Vector2Chunks pos_chunks) : dimension(dimension), pos_chunks(pos_chunks) {}

		void initialise_data();
		void initialise_graphics();
		void enable_data_process();
		void disable_data_process();
		void enable_graphics();
		void disable_graphics();
		void perform_block_process(Seconds delta);
		void perform_render_process(Seconds delta);
		void set_default_ground();
		void set_no_blocks();
	};

	struct Dimension {
		std::string name;

		std::vector<Chunk*> loaded_chunks;

		std::vector<Chunk*> process_running_chunks;
		std::vector<Chunk*> graphic_running_chunks;

		TileDescriptor* default_ground;

		Dimension(std::string name, TileDescriptor* default_ground) : name(name), default_ground(default_ground) {}

		virtual void perform_process(Seconds delta);
		Chunk* get_chunk(Vector2Chunks pos_chunks);
		Chunk* get_load_chunk(Vector2Chunks pos_chunks);
		Chunk* load_chunk(Vector2Chunks pos_chunks);
		bool try_read_chunk(Chunk* chunk);
		virtual void generate_chunk(Chunk* chunk);
		void start_process(Chunk* chunk);
		void start_graphics(Chunk* chunk);
		void end_process(Chunk* chunk);
		void end_graphics(Chunk* chunk);
	};

	struct LoadZone {
		Dimension* dimension;
		std::vector<Chunk*> running_chunks;
		bool run_graphics;

		LoadZone(bool run_graphics, Dimension* dimension) : run_graphics(run_graphics), dimension(dimension) {}

		LoadZone(bool run_graphics, Dimension* dimension, std::span<Vector2Chunks> running_chunks_pos) : run_graphics(run_graphics), dimension(dimension) {
			set_running_chunks(running_chunks_pos);
		}

		void set_running_chunks(std::span<Vector2Chunks> new_running_chunks_pos);
		template <bool run_graphics_const>
		void set_running_chunks_internal(std::span<Vector2Chunks> new_running_chunks_pos);
		void clear();
		~LoadZone();
	};

	struct WorldObject {
		ObjectDescriptor* descriptor_raw;

		WorldObject(Chunk* linked_chunk, Vector2Blocks pos_blocks, ObjectDescriptor* descriptor) : descriptor_raw(descriptor) {
			initialise(linked_chunk, pos_blocks);
		}

		virtual void initialise(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void initialise_data(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void enable_data_process(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void disable_data_process(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void enable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void disable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks);
		virtual void initialise_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks);
	};

	struct ObjectDescriptor {
		ObjectDescriptorId descriptor_id;
		std::string descriptor_name;

		ObjectDescriptor(std::string descriptor_name) : descriptor_name(descriptor_name) {
			bullshit::register_descriptor(this);
		}

		virtual WorldObject* make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) = 0;
	};

	struct Tile : public WorldObject {
		using WorldObject::WorldObject;
	};

	struct TileDescriptor : public ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;

		virtual Tile* make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) = 0;
	};

	struct Block : WorldObject {
		using WorldObject::WorldObject;
	};

	struct BlockDescriptor : ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;

		virtual Block* make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) = 0;
	};

	// CHUNK DEFINITION START

	void Chunk::initialise_data() {
		neighbours[TOP] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(0, 1));
		neighbours[RIGHT] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(1, 0));
		neighbours[BOTTOM] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(0, -1));
		neighbours[LEFT] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(-1, 0));

		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->initialise_data(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->initialise_data(this, { x, y });
				}
			}
		}
	}

	void Chunk::initialise_graphics() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->initialise_graphics(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->initialise_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::enable_data_process() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->enable_data_process(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->enable_data_process(this, { x, y });
				}
			}
		}
	}

	void Chunk::disable_data_process() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->disable_data_process(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->disable_data_process(this, { x, y });
				}
			}
		}
	}

	void Chunk::enable_graphics() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->enable_graphics(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->enable_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::disable_graphics() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y]->disable_graphics(this, { x, y });
			}
		}
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->disable_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::perform_block_process(Seconds delta) {
		for (Process* process : block_processes) {
			process->perform_process(delta);
		}
	}

	void Chunk::perform_render_process(Seconds delta) {
		for (Process* process : render_processes) {
			process->perform_process(delta);
		}
	}

	void Chunk::set_default_ground() {
		TileDescriptor* default_ground = dimension->default_ground;
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				floor_tiles[x][y] = default_ground->make_object(this, Vector2Blocks(x, y));
			}
		}
	}

	void Chunk::set_no_blocks() {
		for (CoordinateBlocks x = 0; x < chunk_size_blocks; ++x) {
			for (CoordinateBlocks y = 0; y < chunk_size_blocks; ++y) {
				blocks[x][y] = nullptr;
			}
		}
	}

	// CHUNK DEFINITION END

	// DIMENSION DEFINITION START

	void Dimension::perform_process(Seconds delta) {
		for (Chunk* chunk : process_running_chunks) {
			chunk->perform_block_process(delta);
		}
		for (Chunk* chunk : graphic_running_chunks) {
			chunk->perform_block_process(delta);
		}
	}

	Chunk* Dimension::get_chunk(Vector2Chunks pos_chunks) {
		for (Chunk* chunk : loaded_chunks) {
			if (chunk->pos_chunks == pos_chunks) {
				return chunk;
			}
		}
		return nullptr;
	}

	Chunk* Dimension::get_load_chunk(Vector2Chunks pos_chunks) {
		for (Chunk* chunk : loaded_chunks) {
			if (chunk->pos_chunks == pos_chunks) {
				return chunk;
			}
		}
		return load_chunk(pos_chunks);
	}

	Chunk* Dimension::load_chunk(Vector2Chunks pos_chunks) {
		Chunk* chunk = new Chunk(this, pos_chunks);
		loaded_chunks.push_back(chunk);
		bool read_from_file = try_read_chunk(chunk);
		if (not read_from_file) {
			generate_chunk(chunk);
		}
		return chunk;
	}

	bool Dimension::try_read_chunk(Chunk* chunk) {
		return false;
	}

	void Dimension::generate_chunk(Chunk* chunk) {
		chunk->set_default_ground();
		chunk->set_no_blocks();
	}

	void Dimension::start_process(Chunk* chunk) {
		if (not chunk->data_initialised) {
			chunk->initialise_data();
		}
		chunk->enable_data_process();
		process_running_chunks.push_back(chunk);
	}

	void Dimension::start_graphics(Chunk* chunk) {
		if (not chunk->graphics_initialised) {
			chunk->initialise_graphics();
		}
		chunk->enable_graphics();
		graphic_running_chunks.push_back(chunk);
	}

	void Dimension::end_process(Chunk* chunk) {
		chunk->disable_data_process();
		std::erase(process_running_chunks, chunk);
	}

	void Dimension::end_graphics(Chunk* chunk) {
		chunk->disable_graphics();
		std::erase(graphic_running_chunks, chunk);
	}

	// DIMENSION DEFINITION END

	// LOADZONE DEFINITION START

	void LoadZone::set_running_chunks(std::span<Vector2Chunks> new_running_chunks_pos) {
		if (run_graphics) {
			set_running_chunks_internal<true>(new_running_chunks_pos);
		}
		else {
			set_running_chunks_internal<false>(new_running_chunks_pos);
		}
	}

	template <bool run_graphics_const>
	void LoadZone::set_running_chunks_internal(std::span<Vector2Chunks> new_running_chunks_pos) {

		std::vector<Chunk*> new_running_chunks;
		for (Vector2Chunks chunk_pos : new_running_chunks_pos) {
			new_running_chunks.push_back(dimension->get_load_chunk(chunk_pos));
		}

		for (Chunk* chunk : running_chunks) {
			if (not can_find_in_vec(new_running_chunks, chunk)) {
				if constexpr (run_graphics_const) {
					if (--(chunk->loading_graphics) == 0) {
						dimension->end_graphics(chunk);
					}
				}
				if (--(chunk->loading_process) == 0) {
					dimension->end_process(chunk);
				}
			}
		}

		for (Chunk* chunk : new_running_chunks) {
			if (not can_find_in_vec(running_chunks, chunk)) {
				if (++(chunk->loading_process) == 1) {
					dimension->start_process(chunk);
				}
				if constexpr (run_graphics_const) {
					if (++(chunk->loading_graphics) == 1) {
						dimension->start_graphics(chunk);
					}
				}
			}
		}
		running_chunks = new_running_chunks;
	}

	void LoadZone::clear() {
		set_running_chunks({});
	}

	LoadZone::~LoadZone() {
		for (Chunk* chunk : running_chunks) {
			if (run_graphics) {
				if (--(chunk->loading_graphics) == 0) {
					dimension->end_graphics(chunk);
				}
			}
			if (--(chunk->loading_process) == 0) {
				dimension->end_process(chunk);
			}
		}
	}

	// LOADZONE DEFINITION END

	// WORLDOBJECT DEFINITION START

	void WorldObject::initialise(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::initialise_data(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::enable_data_process(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::disable_data_process(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::enable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::disable_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	void WorldObject::initialise_graphics(Chunk* linked_chunk, Vector2Blocks pos_blocks) {

	}

	// WORLDOBJECT DEFINITION END


	// SIDE CLASSES EFINITIONS

	struct LoadZoneArea : LoadZone {

		LoadZoneArea(bool run_graphics, Dimension* dimension) : LoadZone(run_graphics, dimension) {

		}

		LoadZoneArea(bool run_graphics, Dimension* dimension, Vector2Chunks center, CoordinateChunks size) : LoadZone(run_graphics, dimension) {
			set(center, size);
		}

		void set(Vector2Chunks center, CoordinateChunks size) {
			std::vector<Vector2Chunks> new_chunks;
			CoordinateChunks one_side = (size * 2 + 1);
			new_chunks.reserve(one_side * one_side);
			size_t counter = 0;
			for (CoordinateChunks x = center.x - size; x <= center.x + size; ++x) {
				for (CoordinateChunks y = center.y - size; y <= center.y + size; ++y) {
					new_chunks.emplace_back(x, y);
					print(x, y, new_chunks.size());
				}
			}
			set_running_chunks(new_chunks);
		}
	};
}