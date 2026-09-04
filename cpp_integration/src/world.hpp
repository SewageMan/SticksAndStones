#pragma once

#include <helper.hpp>
#include <process.hpp>
#include <texture.hpp>
#include <shapes.hpp>

namespace engine {

	typedef uint32_t ObjectDescriptorId;
	typedef uint32_t EntityDescriptorId;
	typedef uint32_t EntityChunkId;
	typedef uint32_t EntytyRunningId;

	constexpr Time entity_idle_timeout = Time::seconds(3);

	struct Chunk;
	struct Dimension;
	struct LoadZone;
	
	struct Collider;

	struct WorldObject;
	struct ObjectDescriptor;

	struct Tile;
	struct TileDescriptor;

	struct Block;
	struct BlockDescriptor;

	struct Entity;
	struct EntityDescriptor;
	struct PhysicsEntity;
	struct PhysicsEntityDescriptor;

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
		void perform_block_process(Time delta);
		void perform_render_process(Time delta);
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

		virtual void perform_process(Time delta);
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

		LoadZone() : run_graphics(false), dimension(nullptr) {}

		LoadZone(bool run_graphics, Dimension* dimension) : run_graphics(run_graphics), dimension(dimension) {}

		LoadZone(bool run_graphics, Dimension* dimension, std::span<Vector2Chunks> running_chunks_pos) : run_graphics(run_graphics), dimension(dimension) {
			set_running_chunks(running_chunks_pos);
		}

		LoadZone& operator=(LoadZone&& other) noexcept {
			if (this != &other) {
				clear();
				dimension = other.dimension;
				running_chunks = std::move(other.running_chunks);
				run_graphics = other.run_graphics;
				other.dimension = nullptr;
				other.running_chunks.clear();
			}
			return *this;
		}

		template <bool run_graphics_const>
		static void chunk_incref(Dimension* dimension, Chunk* chunk);
		template <bool run_grarun_graphics_constphics>
		static void chunk_decref(Dimension* dimension, Chunk* chunk);

		template <bool run_graphics_const>
		static void chunk_incref(Chunk* chunk);
		template <bool run_graphics_const>
		static void chunk_decref(Chunk* chunk);

		void set_running_chunks(std::span<Vector2Chunks> new_running_chunks_pos);
		template <bool run_graphics_const>
		void set_running_chunks_internal(std::span<Vector2Chunks> new_running_chunks_pos);
		void clear();
		~LoadZone();
	};

	struct Collider {
		static void check_collision(Collider& collider1, Collider& collider2) {
			MeshShape* shape1 = collider1.get_shape_collider();
			MeshShape* shape2_copy = collider2.get_shape_collider()->copy();

			Chunk* chunk1 = collider1.get_chunk_collider();
			Chunk* chunk2 = collider2.get_chunk_collider();

			Vector2Chunks chunk_offset = chunk2->pos_chunks - chunk1->pos_chunks;

			shape2_copy->move(chunk_offset * chunk_size_unitsf);

			if (not shape1->is_colliding(*shape2_copy)) {
				delete shape2_copy;
				return;
			}

			Vector2f mtv = shape1->get_mtv(*shape2_copy);
			Vector2f mtv_normalized = mtv.get_normalize();

			delete shape2_copy;

			Vector2Speed speed1 = collider1.get_speed_collider();
			Vector2Speed speed2 = collider2.get_speed_collider();

			Mass mass1 = collider1.get_mass_collider();
			Mass mass2 = collider2.get_mass_collider();

			Speed collision_speed1 = mtv_normalized * speed1;
			Speed collision_speed2 = mtv_normalized * speed2;

			Impulse total_impulse = mass1 * collision_speed1 + mass2 * collision_speed2;

			Speed average_speed = total_impulse / (mass1 + mass2);

			Hardness hardness1 = collider1.get_hardness_collider();
			Hardness hardness2 = collider2.get_hardness_collider();

			Hardness average_hardness = std::min(hardness1, hardness2);

			Speed end_speed1 = average_speed + average_hardness * (collision_speed1 - average_speed);
			Speed end_speed2 = average_speed + average_hardness * (collision_speed2 - average_speed);

			Energy energy1 = mass1 * (end_speed1.square() - collision_speed1.square());
			Energy energy2 = mass2 * (end_speed2.square() - collision_speed2.square());

			energy1.value = std::abs(energy1.value);
			energy2.value = std::abs(energy2.value);

			bool immovable1 = collider1.is_immovable_collider();
			bool immovable2 = collider2.is_immovable_collider();

			if (not (immovable1 and immovable2)) {
				if (immovable1) {
					collider2.move_by_collider(-mtv);
				}
				else if (immovable2) {
					collider1.move_by_collider(mtv);
				}
				else {
					Vector2f half_mtv = mtv / 2;
					collider1.move_by_collider(-half_mtv);
					collider2.move_by_collider(+half_mtv);
				}
			}

			auto [damaged1, destroyed1] = collider1.try_damage_from_energy_collider(energy1);
			auto [damaged2, destroyed2] = collider2.try_damage_from_energy_collider(energy2);

			if (not damaged1 and not damaged2) {
				end_speed1 = average_speed;
				end_speed2 = average_speed;
			}

			if (not destroyed1) {
				Vector2Speed adjusted_vector1 = static_cast<Vector2Speed>(mtv_normalized * (end_speed1 - collision_speed1));
				collider1.set_speed_collider(static_cast<Vector2Speed>(speed1 + adjusted_vector1));
			}
			if (not destroyed2) {
				Vector2Speed adjusted_vector2 = static_cast<Vector2Speed>(mtv_normalized * (end_speed2 - collision_speed2));
				collider2.set_speed_collider(static_cast<Vector2Speed>(speed2 + adjusted_vector2));
			}
		}

		virtual void move_by_collider(Vector2f offset) = 0;

		virtual void set_speed_collider(Vector2Speed speed) = 0;

		virtual std::pair<bool, bool> try_damage_from_energy_collider(Energy energy) = 0;

		virtual Chunk* get_chunk_collider() const = 0;

		virtual MeshShape* get_shape_collider() const = 0;

		virtual Vector2Speed get_speed_collider() const = 0;

		virtual bool is_immovable_collider() const = 0;

		virtual Mass get_mass_collider() const = 0;

		virtual Hardness get_hardness_collider() const = 0;
	};

	struct WorldObject {
		ObjectDescriptor* descriptor_raw;

		WorldObject(Chunk* linked_chunk, Vector2Units pos_blocks, ObjectDescriptor* descriptor) : descriptor_raw(descriptor) {
			initialise(linked_chunk, pos_blocks);
		}

		void initialise(Chunk* linked_chunk, Vector2Units pos_blocks);
		void initialise_data(Chunk* linked_chunk, Vector2Units pos_blocks);
		void enable_data_process(Chunk* linked_chunk, Vector2Units pos_blocks);
		void disable_data_process(Chunk* linked_chunk, Vector2Units pos_blocks);
		void enable_graphics(Chunk* linked_chunk, Vector2Units pos_blocks);
		void disable_graphics(Chunk* linked_chunk, Vector2Units pos_blocks);
		void initialise_graphics(Chunk* linked_chunk, Vector2Units pos_blocks);
	};

	struct ObjectDescriptor {
		ObjectDescriptorId descriptor_id;
		std::string descriptor_name;

		ObjectDescriptor(std::string descriptor_name) : descriptor_name(descriptor_name) {
			bullshit::register_descriptor(this);
		}

		virtual void initialise(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void initialise_data(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void enable_data_process(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void disable_data_process(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void enable_graphics(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void disable_graphics(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);
		virtual void initialise_graphics(WorldObject* world_object, Chunk* linked_chunk, Vector2Units pos_blocks);

		virtual WorldObject* make_object(Chunk* linked_chunk, Vector2Units pos_blocks) = 0;
	};

	struct Tile : public WorldObject {
		using WorldObject::WorldObject;
	};

	struct TileDescriptor : public ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;

		virtual Tile* make_object(Chunk* linked_chunk, Vector2Units pos_blocks) = 0;
	};

	struct Block : WorldObject {
		using WorldObject::WorldObject;
	};

	struct BlockDescriptor : ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;

		virtual Block* make_object(Chunk* linked_chunk, Vector2Units pos_blocks) = 0;
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
		Time sleep_counter = Time::seconds(0);
		EntityChunkId entity_chunk_id;
		EntytyRunningId entity_running_id;

		Entity(Chunk* linked_chunk, Vector2Unitsf pos_units, EntityDescriptor* descriptor) : descriptor_raw(descriptor), pos_units(pos_units), linked_chunk(linked_chunk) {}

		void wake_up() {
			if (not process_running) {
				linked_chunk->dimension->start_process(this);
			}
			else {
				keep_alive();
			}
		}

		void keep_alive() {
			sleep_counter = Time::seconds(0);
		}

		void mark_idle() {
			sleep_counter = entity_idle_timeout;
		}
		
		void start_process();
		void move_by(Vector2Unitsf distance_units);
		void move_by(Vector2Distance distance_units);
		virtual void move_to(Vector2Chunks pos_chunks, Vector2Unitsf pos_units);

		virtual void process(Time delta);
		virtual void pre_initialise();
		virtual void initialise();
		virtual void initialise_data();
		virtual void enable_data_process();
		virtual void disable_data_process();
		virtual void enable_graphics();
		virtual void disable_graphics();
		virtual void initialise_graphics();
	};

	struct EntityDescriptor {
		EntityDescriptorId descriptor_id;
		std::string descriptor_name;

		virtual Entity* make_entity(Chunk* linked_chunk, Vector2Unitsf pos_units) = 0;
	};

	struct HealthEntity : Entity {
		using Entity::Entity;
	};

	struct HealthEntityDescriptor : EntityDescriptor {
		using EntityDescriptor::EntityDescriptor;

		virtual HealthEntity* make_entity(Chunk* linked_chunk, Vector2Unitsf pos_units) = 0;
	};

	struct PhysicsEntity : HealthEntity, Collider {

		using HealthEntity::HealthEntity;

		Mass mass;
		Vector2Speed speed = { Speed::zero(), Speed::zero() };

		void set_speed(Vector2Speed speed) {
			wake_up();
			this->speed = speed;
		}

		virtual void process(Time delta) override {
			move_by(speed * delta);
		}

		PhysicsEntityDescriptor* descriptor() const;
		// defined after descriptor thancks to c++ restrictions

		virtual void set_speed_collider(Vector2Speed speed) override final {
			set_speed(speed);
		};

		virtual void move_by_collider(Vector2f offset) {
			move_by(offset);
		}

		virtual std::pair<bool, bool> try_damage_from_energy_collider(Energy energy) override final {
			return { false, false };
		};

		virtual Chunk* get_chunk_collider() const final override {
			return linked_chunk;
		}

		virtual MeshShape* get_shape_collider() const final override;
		// defined after descriptor thancks to c++ restrictions

		virtual Vector2Speed get_speed_collider() const final override {
			return speed;
		};

		virtual bool is_immovable_collider() const final override {
			return false;
		}

		virtual Mass get_mass_collider() const final override {
			return mass;
		}

		virtual Hardness get_hardness_collider() const final override;
		// defined after descriptor thancks to c++ restrictions
	};

	struct PhysicsEntityDescriptor : HealthEntityDescriptor {
		using HealthEntityDescriptor::HealthEntityDescriptor;

		Hardness entity_hardness;
		MeshShape* entity_shape;

		virtual PhysicsEntity* make_entity(Chunk* linked_chunk, Vector2Unitsf pos_units) = 0;
	};

	PhysicsEntityDescriptor* PhysicsEntity::descriptor() const {
		return static_cast<PhysicsEntityDescriptor*>(descriptor_raw);
	}

	MeshShape* PhysicsEntity::get_shape_collider() const {
		return descriptor()->entity_shape;
	}

	Hardness PhysicsEntity::get_hardness_collider() const {
		return descriptor()->entity_hardness;
	}

	// CHUNK DEFINITION START

	void Chunk::initialise_data() {
		neighbours[UP] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(0, 1));
		neighbours[RIGHT] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(1, 0));
		neighbours[DOWN] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(0, -1));
		neighbours[LEFT] = dimension->get_load_chunk(pos_chunks + Vector2Chunks(-1, 0));

		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->initialise_data(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->initialise_data(this, { x, y });
				}
			}
		}
	}

	void Chunk::initialise_graphics() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->initialise_graphics(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->initialise_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::enable_data_process() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->enable_data_process(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->enable_data_process(this, { x, y });
				}
			}
		}
	}

	void Chunk::disable_data_process() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->disable_data_process(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->disable_data_process(this, { x, y });
				}
			}
		}
	}

	void Chunk::enable_graphics() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->enable_graphics(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->enable_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::disable_graphics() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y]->disable_graphics(this, { x, y });
			}
		}
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				Block* block = blocks[x][y];
				if (block != nullptr) {
					blocks[x][y]->disable_graphics(this, { x, y });
				}
			}
		}
	}

	void Chunk::perform_block_process(Time delta) {
		for (Process* process : block_processes) {
			process->perform_process(delta);
		}
	}

	void Chunk::perform_render_process(Time delta) {
		for (Process* process : render_processes) {
			process->perform_process(delta);
		}
	}

	void Chunk::set_default_ground() {
		TileDescriptor* default_ground = dimension->default_ground;
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				floor_tiles[x][y] = default_ground->make_object(this, Vector2Units(x, y));
			}
		}
	}

	void Chunk::set_no_blocks() {
		for (CoordinateUnits x = 0; x < chunk_size_units; ++x) {
			for (CoordinateUnits y = 0; y < chunk_size_units; ++y) {
				blocks[x][y] = nullptr;
			}
		}
	}

	// CHUNK DEFINITION END

	// DIMENSION DEFINITION START

	void Dimension::perform_process(Time delta) {
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
		print("new_chunk_loaded", pos_chunks);
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
		entity->keep_alive();
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

	template <bool run_graphics_const>
	inline void LoadZone::chunk_incref(Dimension* dimension, Chunk* chunk) {
		print("chunk_incref", chunk->pos_chunks, "proc:",chunk->loading_process, "graph:", chunk->loading_graphics, "graph_load:", run_graphics_const);
		if (++(chunk->loading_process) == 1) {
			dimension->start_process(chunk);
		}
		if constexpr (run_graphics_const) {
			if (++(chunk->loading_graphics) == 1) {
				dimension->start_graphics(chunk);
			}
		}
		print("after_chunk_incref", chunk->pos_chunks, "proc:", chunk->loading_process, "graph:", chunk->loading_graphics, "graph_load:", run_graphics_const);
		print(" ");
	}
	template <bool run_graphics_const>
	inline void LoadZone::chunk_decref(Dimension* dimension, Chunk* chunk) {
		print("chunk_decref", chunk->pos_chunks, "proc:", chunk->loading_process, "graph:", chunk->loading_graphics, "graph_load:", run_graphics_const);
		if constexpr (run_graphics_const) {
			if (--(chunk->loading_graphics) == 0) {
				dimension->end_graphics(chunk);
			}
		}
		if (--(chunk->loading_process) == 0) {
			dimension->end_process(chunk);
		}
		print("after_chunk_decref", chunk->pos_chunks, "proc:", chunk->loading_process, "graph:", chunk->loading_graphics, "graph_load:", run_graphics_const);
		print(" ");
	}

	template <bool run_graphics_const>
	inline void LoadZone::chunk_incref(Chunk* chunk) {
		chunk_incref<run_graphics_const>(chunk->dimension, chunk);
	}
	template <bool run_graphics_const>
	inline void LoadZone::chunk_decref(Chunk* chunk) {
		chunk_decref<run_graphics_const>(chunk->dimension, chunk);
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
		print("!!!!!!!! deleting zone");
		if (run_graphics) {
			for (Chunk* chunk : running_chunks) {
				this->chunk_decref<true>(dimension, chunk);
			}
		}
		else {
			for (Chunk* chunk : running_chunks) {
				this->chunk_decref<false>(dimension, chunk);
			}
		}
		
	}

	// LOADZONE DEFINITION END

	// WORLDOBJECT DEFINITION START

	void WorldObject::initialise(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->initialise(this, linked_chunk, pos_blocks);
	}

	void WorldObject::initialise_data(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->initialise_data(this, linked_chunk, pos_blocks);
	}

	void WorldObject::enable_data_process(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->enable_data_process(this, linked_chunk, pos_blocks);
	}

	void WorldObject::disable_data_process(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->disable_data_process(this, linked_chunk, pos_blocks);
	}

	void WorldObject::enable_graphics(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->enable_graphics(this, linked_chunk, pos_blocks);
	}

	void WorldObject::disable_graphics(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->disable_graphics(this, linked_chunk, pos_blocks);
	}

	void WorldObject::initialise_graphics(Chunk* linked_chunk, Vector2Units pos_blocks) {
		descriptor_raw->initialise_graphics(this, linked_chunk, pos_blocks);
	}

	// WORLDOBJECT DEFINITION END

	// OBJECTDESCRIPTOR DEFINITION START

	void ObjectDescriptor::initialise(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::initialise_data(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::enable_data_process(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::disable_data_process(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::enable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::disable_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	void ObjectDescriptor::initialise_graphics(WorldObject* world_obejct, Chunk* linked_chunk, Vector2Units pos_blocks) {

	}

	// OBJECTDESCRIPTOR DEFINITION END

	// ENTITY DEFINITION START

	void Entity::start_process() {
		linked_chunk->dimension->start_process(this);
	}

	void Entity::move_by(Vector2Unitsf distance_units) {
		auto [pos_chunks, pos_units] = snap_to_chunk(this->pos_units + distance_units);
		move_to(linked_chunk->pos_chunks + pos_chunks, pos_units);
	}

	void Entity::move_by(Vector2Distance distance_units) {
		move_by(Vector2Unitsf(distance_units.x.value, distance_units.y.value));
	}

	void Entity::move_to(Vector2Chunks pos_chunks, Vector2Unitsf pos_units) {
		wake_up();
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
					direction = diff_chunks.y > 0 ? UP : DOWN;
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
		wake_up();
	}

	void Entity::process(Time delta) {
		mark_idle();
	}

	void Entity::initialise() {

	}

	void Entity::initialise_data() {

	}

	void Entity::enable_data_process() {
		LoadZone::chunk_incref<false>(linked_chunk);
	}

	void Entity::disable_data_process() {
		LoadZone::chunk_decref<false>(linked_chunk);
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

		LoadZoneArea() {}

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
				}
			}
			set_running_chunks(new_chunks);
		}
	};
}