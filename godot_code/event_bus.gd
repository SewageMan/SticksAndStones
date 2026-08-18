extends RefCounted

class_name EventBus

var channel_ids: Array[int] = []
var bus_ids: Array[int] = []


func subscribe(channel_id: int) -> void:
	var bus_id: int = Global.cpp_core.subscribe_event_bus(channel_id, self)
	channel_ids.append(channel_id)
	bus_ids.append(bus_id)


func unsubscribe(channel_id: int) -> void:
	var index: int = channel_ids.find(channel_id)
	Global.cpp_core.unsubscribe_event_bus(channel_ids[index], bus_ids[index])
	channel_ids.remove_at(index)
	bus_ids.remove_at(index)


func send(channel_id: int, payload: GodotByteBuffer) -> bool:
	return Global.cpp_core.send(channel_id, payload)


func receive(channel_id: int, payload: GodotByteBuffer) -> bool:
	return false


func unsubscribe_all() -> void:
	for index: int in range(channel_ids.size()):
		Global.cpp_core.unsubscribe_event_bus(channel_ids[index], bus_ids[index])
	channel_ids.clear()
	bus_ids.clear()


func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		for index: int in range(channel_ids.size()):
			Global.cpp_core.unsubscribe_event_bus(channel_ids[index], bus_ids[index])
		channel_ids.clear()
		bus_ids.clear()
