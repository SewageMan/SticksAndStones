#pragma once

#include <world.hpp>
#include <event_bus.hpp>

namespace engine {

	namespace bullshit {
		Vector2Chunks get_camera_pos_chunks();
		void set_camera_pos(Vector2Chunks pos_chunks, Vector2Unitsf pos_units);
		float get_camera_zoom();
		void set_camera_zoom(float new_zoom);
	}

	struct Player;

	struct PlayerEventBus : EventBus {
		Player* player;

		PlayerEventBus(Player* player) : EventBus(event_bus_ids::SET_PLAYER_SPEED, event_bus_ids::MULTIPLY_CAMERA_ZOOM), player(player) {}

		virtual bool receive(EventChannelId channel_id, EventPackage payload) override;

		void send_player_state();
	};

	struct Player : PhysicsEntity {
		using PhysicsEntity::PhysicsEntity;
		
		PlayerEventBus event_bus = PlayerEventBus(this);
		LoadZoneArea load_zone;

		static constexpr uint32_t load_radius = 1;

		virtual void initialise() override {
			print("process_running",process_running);
			print("load zone start",load_zone.dimension, load_zone.run_graphics, load_zone.running_chunks.size());
			load_zone = LoadZoneArea(true, linked_chunk->dimension, linked_chunk->pos_chunks, load_radius);
		}

		virtual void process(Time delta) override {
			PhysicsEntity::process(delta);

			load_zone.set(linked_chunk->pos_chunks, load_radius);

			bullshit::set_camera_pos(linked_chunk->pos_chunks, pos_units);

			event_bus.send_player_state();
		}
	};

	void PlayerEventBus::send_player_state() {
		ByteBuffer buffer = ByteBuffer(16);

		Vector2Chunks camera_pos_chunks = bullshit::get_camera_pos_chunks();
		Vector2Unitsf offset_pos_units = player->pos_units + chunk_size_units * (camera_pos_chunks - player->linked_chunk->pos_chunks);

		buffer.write(offset_pos_units);
		buffer.write(player->speed);

		EventPackage payload;
		payload.pointer = buffer.data;
		send(event_bus_ids::SET_PLAYER_STATE, payload);
	}

	bool PlayerEventBus::receive(EventChannelId channel_id, EventPackage payload) {
		if (channel_id == event_bus_ids::SET_PLAYER_SPEED) {
			player->set_speed(static_cast<Vector2Speed>(payload.vector2f));
			return true;
		}
		else if (channel_id == event_bus_ids::MULTIPLY_CAMERA_ZOOM) {
			float zoom = bullshit::get_camera_zoom();
			zoom *= payload.float_val;
			bullshit::set_camera_zoom(zoom);
			return true;
		}
		return false;
	}

	struct PlayerDescriptor : PhysicsEntityDescriptor {
		using PhysicsEntityDescriptor::PhysicsEntityDescriptor;

		virtual Player* make_entity(Chunk* linked_chunk, Vector2Unitsf pos_units) override {
			Player* player = new Player(linked_chunk, pos_units, this);
			player->pre_initialise();
			player->initialise();
			return player;
		}
	};

}