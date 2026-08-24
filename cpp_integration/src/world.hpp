#pragma once

#include <helper.hpp>
#include <process.hpp>
#include <texture.hpp>

namespace engine {

	typedef uint32_t ObjectDescriptorId;
	typedef uint32_t EntityChunkId;
	typedef uint32_t EntytyRunningId;

	constexpr Seconds entity_idle_timeout = 3;

	struct Chunk;
	struct Dimension;
	struct LoadZone;
	
	struct WorldObject;
	struct ObjectDescriptor;

	struct Tile;
	struct TileDescriptor;

	struct Block;
	struct BlockDescriptor;

	struct Entity;
	struct EntityDescriptor;

	namespace bullshit {
		void register_descriptor(ObjectDescriptor* descriptor);
	}

	struct Chunk {
		Dimension* const dimension;

		const Vector2Chunks pos_chunks;

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
		PersistentContainer<Entity*, EntityChunkId> entities;

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
		const std::string name;

		std::vector<Chunk*> loaded_chunks;

		std::vector<Chunk*> process_running_chunks;
		std::vector<Chunk*> graphic_running_chunks;

		PersistentContainer<Entity*, EntytyRunningId> running_entities;

		TileDescriptor* default_ground;

		Dimension(std::string name, TileDescriptor* default_ground) : name(name), default_ground(default_ground) {}

		virtual void perform_process(Seconds delta);
		Chunk* get_chunk(Vector2Chunks pos_chunks);
		Chunk* get_load_chunk(Vector2Chunks pos_chunks);
		Chunk* get_load_chunk_initialised(Vector2Chunks pos_chunks);
		Chunk* load_chunk(Vector2Chunks pos_chunks);
		bool try_read_chunk(Chunk* chunk);
		virtual void generate_chunk(Chunk* chunk);
		void start_process(Chunk* chunk);
		void start_graphics(Chunk* chunk);
		void end_process(Chunk* chunk);
		void end_graphics(Chunk* chunk);
		void start_process(Entity* entity);
		void start_graphics(Entity* entity);
		void end_process(Entity* entity);
		void end_graphics(Entity* entity);
	};

	struct LoadZone {
		Dimension* dimension;
		std::vector<Chunk*> running_chunks;
		bool run_graphics;

		LoadZone(bool run_graphics, Dimension* dimension) : run_graphics(run_graphics), dimension(dimension) {}

		LoadZone(bool run_graphics, Dimension* dimension, std::span<Vector2Chunks> running_chunks_pos) : run_graphics(run_graphics), dimension(dimension) {
			set_running_chunks(running_chunks_pos);
		}

		template <bool run_graphics>
		static void chunk_incref(Dimension* dimension, Chunk* chunk);
		template <bool run_graphics>
		static void chunk_decref(Dimension* dimension, Chunk* chunk);

		template <bool run_graphics>
		static void chunk_incref(Chunk* chunk);
		template <bool run_graphics>
		static void chunk_decref(Chunk* chunk);

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

	struct Entity {
		constexpr static CoordinateChunks maximum_stepping_distance = 3;

		EntityDescriptor* descriptor_raw;
		Vector2Unitsf pos_units;
		Chunk* linked_chunk;
		bool data_initialised = false;
		bool graphics_initialised = false;
		bool process_running = false;
		bool graphics_running = false;
		Seconds sleep_counter = 0;
		EntityChunkId entity_chunk_id;
		EntytyRunningId entity_running_id;

		Entity(Chunk* linked_chunk, Vector2Unitsf pos_units, EntityDescriptor* descriptor) : descriptor_raw(descriptor), pos_units(pos_units), linked_chunk(linked_chunk) {
			pre_initialise();
			initialise();
		}

		void mark_alive() {
			sleep_counter = 0;
		}

		void mark_idle() {
			sleep_counter = entity_idle_timeout;
		}
		
		void start_process();
		void move_by(Vector2Unitsf distance_units);
		virtual void move_to(Vector2Chunks pos_chunks, Vector2Unitsf pos_units);

		virtual void process(Seconds delta);
		virtual void pre_initialise();
		virtual void initialise();
		virtual void initialise_data();
		virtual void enable_data_process();
		virtual void disable_data_process();
		virtual void enable_graphics();
		virtual void disable_graphics();
		virtual void initialise_graphics();
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
			chunk->perform_render_process(delta);
		}
		for (Entity* entity : running_entities.snapshot()) {
			entity->sleep_counter += delta;
			entity->process(delta);
			if (entity->sleep_counter >= entity_idle_timeout) {
				end_process(entity);
			}
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

	Chunk* Dimension::get_load_chunk_initialised(Vector2Chunks pos_chunks) {
		Chunk* chunk = get_load_chunk(pos_chunks);
		if (not chunk->data_initialised) {
			chunk->initialise_data();
		}
		return chunk;
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
		chunk->process_running = true;
		process_running_chunks.push_back(chunk);
	}

	void Dimension::start_graphics(Chunk* chunk) {
		if (not chunk->graphics_initialised) {
			chunk->initialise_graphics();
		}
		chunk->enable_graphics();
		chunk->graphics_running = true;
		graphic_running_chunks.push_back(chunk);
	}

	void Dimension::end_process(Chunk* chunk) {
		chunk->disable_data_process();
		chunk->process_running = false;
		std::erase(process_running_chunks, chunk);
	}

	void Dimension::end_graphics(Chunk* chunk) {
		chunk->disable_graphics();
		chunk->graphics_running = false;
		std::erase(graphic_running_chunks, chunk);
	}

	void Dimension::start_process(Entity* entity) {
		if (not entity->data_initialised) {
			entity->initialise_data();
		}
		entity->enable_data_process();
		entity->process_running = true;
		entity->entity_running_id = running_entities.add_element(entity);
	}

	void Dimension::start_graphics(Entity* entity) {
		if (not entity->graphics_initialised) {
			entity->initialise_graphics();
		}
		entity->enable_graphics();
		entity->graphics_running = true;
	}

	void Dimension::end_process(Entity* entity) {
		entity->disable_data_process();
		entity->process_running = false;
		running_entities.delete_element(entity->entity_running_id);
	}

	void Dimension::end_graphics(Entity* entity) {
		entity->disable_graphics();
		entity->graphics_running = false;
	}

	// DIMENSION DEFINITION END

	// LOADZONE DEFINITION START

	template <bool run_graphics>
	inline void LoadZone::chunk_incref(Dimension* dimension, Chunk* chunk) {
		if (++(chunk->loading_process) == 1) {
			dimension->start_process(chunk);
		}
		if constexpr (run_graphics) {
			if (++(chunk->loading_graphics) == 1) {
				dimension->start_graphics(chunk);
			}
		}
	}
	template <bool run_graphics>
	inline void LoadZone::chunk_decref(Dimension* dimension, Chunk* chunk) {
		if constexpr (run_graphics) {
			if (--(chunk->loading_graphics) == 0) {
				dimension->end_graphics(chunk);
			}
		}
		if (--(chunk->loading_process) == 0) {
			dimension->end_process(chunk);
		}
	}

	template <bool run_graphics>
	inline void LoadZone::chunk_incref(Chunk* chunk) {
		chunk_incref<run_graphics>(chunk->dimension, chunk);
	}
	template <bool run_graphics>
	inline void LoadZone::chunk_decref(Chunk* chunk) {
		chunk_decref<run_graphics>(chunk->dimension, chunk);
	}

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
				this->chunk_decref<run_graphics_const>(dimension, chunk);
			}
		}

		for (Chunk* chunk : new_running_chunks) {
			if (not can_find_in_vec(running_chunks, chunk)) {
				this->chunk_incref<run_graphics_const>(dimension, chunk);
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

	// ENTITY DEFINITION START

	void Entity::start_process() {
		linked_chunk->dimension->start_process(this);
	}

	void Entity::move_by(Vector2Unitsf distance_units) {
		auto [pos_chunks, pos_units] = snap_to_chunk(this->pos_units + distance_units);
		move_to(linked_chunk->pos_chunks + pos_chunks, pos_units);
	}

	void Entity::move_to(Vector2Chunks pos_chunks, Vector2Unitsf pos_units) {
		Vector2Chunks cur_pos_chunks = linked_chunk->pos_chunks;
		Vector2Chunks diff_chunks = pos_chunks - cur_pos_chunks;
		if (not diff_chunks.is_zero()) {
			Chunk* current_chunk;
			if (diff_chunks.get_abs().sum() <= maximum_stepping_distance) {
				Vector2Chunks dif_abs = diff_chunks.get_abs();
				current_chunk = linked_chunk;
				Direction direction = diff_chunks.x > 0 ? RIGHT : LEFT;
				for (uint32_t x_step = 0; x_step < dif_abs.x; ++x_step) {
					current_chunk = current_chunk->neighbours[direction];
					if (current_chunk == nullptr) {
						break;
					}
				}
				if (current_chunk != nullptr) {
					direction = diff_chunks.y > 0 ? TOP : BOTTOM;
					for (uint32_t y_step = 0; y_step < dif_abs.y; ++y_step) {
						current_chunk = current_chunk->neighbours[direction];
						if (current_chunk == nullptr) {
							current_chunk = linked_chunk->dimension->get_load_chunk_initialised(pos_chunks);
							break;
						}
					}
				}
				else {
					current_chunk = linked_chunk->dimension->get_load_chunk_initialised(pos_chunks);
				}
			}
			else {
				current_chunk = linked_chunk->dimension->get_load_chunk_initialised(pos_chunks);
			}
			Chunk* old_chunk = linked_chunk;
			linked_chunk = current_chunk;

			old_chunk->entities.delete_element(entity_chunk_id);
			entity_chunk_id = current_chunk->entities.add_element(this);

			LoadZone::chunk_incref<false>(current_chunk);
			LoadZone::chunk_decref<false>(old_chunk);
		}
		this->pos_units = pos_units;
	}

	void Entity::pre_initialise() {
		entity_chunk_id = linked_chunk->entities.add_element(this);
		LoadZone::chunk_incref<false>(linked_chunk);
	}

	void Entity::process(Seconds delta) {
		mark_idle();
	}

	void Entity::initialise() {

	}

	void Entity::initialise_data() {

	}

	void Entity::enable_data_process() {

	}

	void Entity::disable_data_process() {

	}

	void Entity::enable_graphics() {

	}

	void Entity::disable_graphics() {

	}

	void Entity::initialise_graphics() {

	}

	// ENTITY DEFINITION END

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