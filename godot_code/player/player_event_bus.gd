extends EventBus

class_name PlayerEventBus

var player: Player


func _init(player_in: Player):
	print("starting")
	subscribe(Global.cpp_core.SET_PLAYER_STATE)
	print("first")
	subscribe(Global.cpp_core.UPDATE_CAMERA_CHUNK)
	print("second")
	self.player = player_in


func receive(channel_id: int, payload: GodotByteBuffer) -> bool:
	if channel_id == Global.cpp_core.SET_PLAYER_STATE:
		var buffer: GodotByteBuffer = payload.dereference_data(16)
		var position: Vector2 = buffer.read_vector2f()
		var speed: Vector2 = buffer.read_vector2f()
		buffer.deallocate()

		player.position = position * Global.world_scale
		player.velocity = speed * Global.world_scale
		return true
	elif channel_id == Global.cpp_core.UPDATE_CAMERA_CHUNK:
		var chunk_offset: Vector2i = payload.read_vector2i()
		var unit_offset: Vector2 = chunk_offset * Global.chunk_size_units * Global.world_scale
		print(chunk_offset, unit_offset, "camera changed chunk")
		player.position += unit_offset
		return true
	return false


func send_player_speed(speed: Vector2) -> void:
	var payload: GodotByteBuffer = GodotByteBuffer.allocate()
	payload.initialise(8)
	payload.write_vector2f(speed)
	send(Global.cpp_core.SET_PLAYER_SPEED, payload)
	payload.deallocate()


func multiply_camera_zoom(multiplier: float):
	var payload: GodotByteBuffer = GodotByteBuffer.allocate()
	payload.initialise(8)
	payload.write_float(multiplier)
	send(Global.cpp_core.MULTIPLY_CAMERA_ZOOM, payload)
	payload.deallocate()
