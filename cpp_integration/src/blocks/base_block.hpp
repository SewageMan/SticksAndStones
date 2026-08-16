#pragma once

#include <base_world_object.hpp>


namespace engine {

	struct Block : WorldObject {
		using WorldObject::WorldObject;
	};

	struct BlockDescriptor : ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;
	};
}