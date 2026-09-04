#pragma once

#include <tiles/simple_tile.hpp>
#include <player.hpp>

class CppCore;

namespace engine {

	namespace dimensions_n {
		Dimension* overworld;
	}

	namespace blocks {
		
	}

	namespace tiles {
		SimpleTileDescriptor* test_ground;
	}

	namespace textures {
		Texture test;
	}

	struct MainEngine {
		std::vector<Dimension*> dimensions;

		std::vector<ObjectDescriptor*> descriptors;
		std::unordered_map<std::string, ObjectDescriptorId> descriptor_names;

		void initialise() {

			enable_fast_math_hardware_modes();

			textures::test = Texture("res://loser-city-enjoyer-v0-mph9c839dstd1.webp");

			tiles::test_ground = new SimpleTileDescriptor("test_ground", textures::test);

			dimensions_n::overworld = new Dimension("overworld", tiles::test_ground);
			add_dimension(dimensions_n::overworld);

			new LoadZoneArea(true, dimensions_n::overworld, {2,-1}, 2);

			PlayerDescriptor* player_descriptor = new PlayerDescriptor();

			Player* player = player_descriptor->make_entity(dimensions_n::overworld->get_load_chunk_initialised({ 0,0 }), { 0,0 });
		}

		void add_dimension(Dimension* dimension) {
			for (Dimension* dimension_iter : dimensions) {
				if (dimension->name == dimension_iter->name) {
					panic("ERROR: trying to add dimension with name: '" + dimension->name + "' but other dimension already has this name");
				}
			}
			dimensions.push_back(dimension);
		}

		void perform_process(Time delta) {
			for (Dimension* dimension : dimensions) {
				dimension->perform_process(delta);
			}
		}

		static MainEngine instance;
	};

	MainEngine MainEngine::instance;

	namespace bullshit {
		void register_descriptor(ObjectDescriptor* descriptor) {
			ObjectDescriptorId descriptor_id = MainEngine::instance.descriptors.size();
			descriptor->descriptor_id = descriptor_id;
			MainEngine::instance.descriptors.push_back(descriptor);
			MainEngine::instance.descriptor_names.emplace(descriptor->descriptor_name, descriptor_id);
		}
	}
}