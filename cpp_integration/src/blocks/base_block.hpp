#pragma once

#include <base_world_object.hpp>


namespace engine {

	struct BlockRender : ObjectRender {
		using ObjectRender::ObjectRender;
	};

	struct Block : WorldObject {
		using WorldObject::WorldObject;
	};

	struct BlockDescriptor : ObjectDescriptor {
		using ObjectDescriptor::ObjectDescriptor;
	};
}