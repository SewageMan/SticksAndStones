#pragma once

#include <helper.hpp>
#include <process.hpp>
#include <texture.hpp>

namespace engine {

	typedef uint32_t ObjectDescriptorId;

	struct Chunk;

	struct ObjectRender;

	struct WorldObject;

	struct ObjectDescriptor;

	namespace bullshit {
		void register_block_process(Chunk* block, Process* process);
		void register_render_process(Chunk* render, Process* process);
		void register_descriptor(ObjectDescriptor* descriptor);
	}

	struct ChunkStorage {
		Chunk* linked_chunk;

		ChunkStorage(Chunk* linked_chunk) : linked_chunk(linked_chunk) {}

		void register_process(Process* process) {
			register_block_process(process);
		}

		void register_block_process(Process* process) {
			bullshit::register_block_process(linked_chunk, process);
		}

		void register_render_process(Process* process) {
			bullshit::register_render_process(linked_chunk, process);
		}
	};

	struct ObjectRender {
		WorldObject* linked_object;

		ObjectRender(WorldObject* linked_object) : linked_object(linked_object) {}
	};

	struct WorldObject {
		ObjectDescriptor* descriptor;
		ObjectRender* linked_render = nullptr;
		Vector2Block pos_blocks;

		WorldObject(Chunk* linked_chunk, ObjectDescriptor* descriptor, Vector2Block pos_blocks) : descriptor(descriptor), pos_blocks(pos_blocks) {
			initialise();
		}

		virtual void initialise() {

		}

		virtual void initialise_data() {

		}

		virtual void initialise_graphics() {
			linked_render = initialise_render();
		}

		virtual ObjectRender* initialise_render() {
			return new ObjectRender(this);
		}

		Vector2Units pos_units() {
			return pos_blocks * block_size;
		}
	};

	struct ObjectDescriptor {
		ObjectDescriptorId descriptor_id;
		std::string descriptor_name;

		ObjectDescriptor(std::string descriptor_name) : descriptor_name(descriptor_name) {
			bullshit::register_descriptor(this);
		}

		virtual WorldObject* make_object(Chunk* linked_chunk, Vector2Block pos_blocks) {
			return new WorldObject(linked_chunk, this, pos_blocks);
		}
	};
}