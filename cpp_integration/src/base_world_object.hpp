#pragma once

#include <helper.hpp>
#include <process.hpp>
#include <texture.hpp>

namespace engine {

	namespace bullshit {
		BlockProcessId register_block_process(Chunk* block, Process* process);
		RenderProcessId register_render_process(Chunk* render, Process* process);
		void register_descriptor(ObjectDescriptor* descriptor);
	}

	struct TemplatedChunkStorage {
		Chunk* linked_chunk;

		ChunkStorage(Chunk* linked_chunk) : linked_chunk(linked_chunk) {}

		BlockProcessId register_block_process(Process* process) {
			bullshit::register_block_process(linked_chunk, process);
		}

		RenderProcessId register_render_process(Process* process) {
			bullshit::register_render_process(linked_chunk, process);
		}
	};

	template <typename = void>
	struct TemplatedWorldObject {
		ObjectDescriptor* descriptor_raw;
		Vector2Blocks pos_blocks;

		WorldObject(Chunk* linked_chunk, ObjectDescriptor* descriptor, Vector2Blocks pos_blocks) : descriptor_raw(descriptor), pos_blocks(pos_blocks) {
			initialise();
		}

		virtual void initialise() {

		}

		virtual void initialise_data() {

		}

		virtual void enable_data_process() {

		}

		virtual void disable_data_process() {

		}

		virtual void enable_graphics() {

		}

		virtual void disable_graphics() {

		}

		virtual void initialise_graphics() {
			
		}

		Vector2Units pos_units() {
			return pos_blocks * block_size;
		}
	};

	template <typename = void>
	struct TemplatedObjectDescriptor {
		ObjectDescriptorId descriptor_id;
		std::string descriptor_name;

		ObjectDescriptor(std::string descriptor_name) : descriptor_name(descriptor_name) {
			bullshit::register_descriptor(this);
		}

		virtual WorldObject* make_object(Chunk* linked_chunk, Vector2Blocks pos_blocks) {
			return new WorldObject(linked_chunk, this, pos_blocks);
		}
	};

}