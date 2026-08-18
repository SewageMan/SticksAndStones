#pragma once

#include <helper.hpp>

namespace engine {

	typedef uint32_t EventBusId;
	typedef uint32_t EventChannelId;

	struct EventBus;

	union EventPackage {
		bool bool_val;
		uint8_t chars[sizeof(void*)];
		uint32_t uint32;
		int32_t int32;
		int64_t int64;
		float float_val;
		double double_val;
		Vector2i vector2i;
		Vector2f vector2f;
		void* pointer;
	};

	namespace bullshit {
		EventBusId subscribe(EventChannelId channel_id, EventBus* event_bus);
		void unsubscribe(EventChannelId channel_id, EventBusId event_bus_id);
		bool send(EventChannelId channel_id, EventPackage payload);
	}

	struct EventBus {
		std::vector<EventChannelId> channel_ids;
		std::vector<EventBusId> bus_ids;

		EventBus() {
			
		}

		template <typename... Args>
		EventBus(Args... channel_ids) {
			(subscribe(channel_ids), ...);
		}

		void subscribe(EventChannelId channel_id) {
			channel_ids.push_back(channel_id);
			bus_ids.push_back(bullshit::subscribe(channel_id, this));
		}

		void unsubscribe(EventChannelId channel_id) {
			auto found = std::find(channel_ids.begin(), channel_ids.end(), channel_id);
			if (found == channel_ids.end()) {
				panic("did not find channel to unsubscribe inside the event bus subscribed to it");
			}
			size_t index = std::distance(channel_ids.begin(), found);
			EventBusId bus_id = bus_ids[index];
			bullshit::unsubscribe(channel_id, bus_id);
			channel_ids.erase(channel_ids.begin() + index);
			bus_ids.erase(bus_ids.begin() + index);
		}

		bool send(EventChannelId channel_id, EventPackage payload) {
			return bullshit::send(channel_id, payload);
		}

		virtual bool receive(EventChannelId channel_id, EventPackage payload) {
			return false;
		}

		EventBus& operator=(const EventBus&) = delete;

		EventBus(const EventBus&) = delete;

		~EventBus() {
			for (size_t id = 0; id < bus_ids.size(); ++id) {
				bullshit::unsubscribe(channel_ids[id], bus_ids[id]);
			}
		}
	};
}