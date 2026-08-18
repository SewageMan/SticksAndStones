extends EventBus

class_name TestEventBus


func _init():
	subscribe(1)


func receive(channel_id: int, payload: GodotByteBuffer) -> bool:
	var received: int = payload.read_uint32()
	print("received integer in godot:", received)
	if received > 0:
		var payload_send: GodotByteBuffer = payload.copy()
		payload_send.write_uint32(received - 1)
		return send(0, payload_send)
	return true
