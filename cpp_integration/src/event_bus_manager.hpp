#pragma once

#include <godot_helper.hpp>
#include <event_bus.hpp>
#include <godot_byte_buffer.hpp>

namespace engine {

    union EventBusMixed {
        EventBus* cpp_bus;
        godot::Object* godot_bus;
    };

    enum EventBusSide {
        cpp_side = 0,
        godot_side = 1
    };

    struct EventBusContainer {
        EventBusMixed event_bus;
        EventBusSide side;

        EventBusContainer() {}

        EventBusContainer(EventBus* event_bus) : side(cpp_side) {
            this->event_bus.cpp_bus = event_bus;
        }

        EventBusContainer(godot::Object* event_bus) : side(godot_side) {
            this->event_bus.godot_bus = event_bus;
        }
    };

    struct EventBusManager {

        std::vector<PersistentContainer<EventBusContainer, EventBusId>> event_channels;
        std::vector<uint32_t> event_godot_subscribers;

        void initialise() {

        }

        EventBusId subscribe(EventChannelId channel_id, EventBus* event_bus) {
            ensure_channel_exists(channel_id);
            return event_channels[channel_id].emplace_element(event_bus);
        }

        EventBusId subscribe_godot(EventChannelId channel_id, godot::Object* event_bus) {
            ensure_channel_exists(channel_id);
            event_godot_subscribers[channel_id] += 1;
            return event_channels[channel_id].emplace_element(event_bus);
        }

        void unsubscribe(EventChannelId channel_id, EventBusId event_bus_id) {
            event_channels[channel_id].delete_element(event_bus_id);
        }

        void unsubscribe_godot(EventChannelId channel_id, EventBusId event_bus_id) {
            event_godot_subscribers[channel_id] -= 1;
            event_channels[channel_id].delete_element(event_bus_id);
        }

        bool send(EventChannelId channel_id, EventPackage payload) {
            ensure_channel_exists(channel_id);
            godot::GodotByteBuffer* godot_byte_buffer = nullptr;
            bool has_godot_subscribers = event_godot_subscribers[channel_id] != 0;
            if (has_godot_subscribers) {
                godot_byte_buffer = godot::GodotByteBuffer::allocate_godot_byte_buffer();
                godot_byte_buffer->byte_buffer.set_values(reinterpret_cast<uint8_t*>(&payload), sizeof(EventPackage));
            }
            bool handled = false;
            for (EventBusContainer target : event_channels[channel_id].snapshot()) {
                if (target.side == cpp_side) {
                    EventBus* target_bus = target.event_bus.cpp_bus;
                    if (target_bus->receive(channel_id, payload)) {
                        handled = true;
                    }
                }
                else {
                    godot_byte_buffer->byte_buffer.offset = 0;
                    godot::Object* target_bus = target.event_bus.godot_bus;
                    godot::Variant result = target_bus->call("receive", channel_id, godot_byte_buffer);
                    if (result.operator bool()) {
                        handled = true;
                    }
                }
            }
            if (has_godot_subscribers) {
                godot::GodotByteBuffer::free_godot_byte_buffer(godot_byte_buffer);
            }
            return handled;
        }

        void ensure_channel_exists(EventChannelId channel_id) {
            if (event_channels.size() <= channel_id) {
                event_channels.resize(channel_id + 1);
                event_godot_subscribers.resize(channel_id + 1, 0);
            }
        }

        static EventBusManager instance;
    };

    EventBusManager EventBusManager::instance;

    namespace bullshit {
        EventBusId subscribe(EventChannelId channel_id, EventBus* event_bus) {
            return EventBusManager::instance.subscribe(channel_id, event_bus);
        }
        void unsubscribe(EventChannelId channel_id, EventBusId event_bus_id) {
            return EventBusManager::instance.unsubscribe(channel_id, event_bus_id);
        }
        bool send(EventChannelId channel_id, EventPackage payload) {
            return EventBusManager::instance.send(channel_id, payload);
        }
    }
}