#pragma once

#include <helper.hpp>

namespace engine {
	struct Process {
		virtual void perform_process(Seconds delta) = 0;
	};
}