#pragma once

#include <dimension.hpp>

namespace engine {
	struct MainEngine {
		std::vector<Dimension*> dimensions;

		void add_dimension(Dimension* dimension) {
			for (Dimension* dimension_iter : dimensions) {
				if (dimension->name == dimension_iter->name) {
					panic("ERROR: trying to add dimension with name: '" + dimension->name + "' but other dimension already has this name");
				}
			}
			dimensions.push_back(dimension);
			dimension->on_engine_add(this);
		}
	};
}