#pragma once

#include <helper.hpp>

namespace engine {

	typename size_t ProcessId;

	typedef size_t RenderProcessId;
	typedef size_t BlockProcessId;

	struct Process {
		virtual void perform_process(Time delta) = 0;
	};
}