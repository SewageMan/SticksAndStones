extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var cpp_core = CppGodotAdapter.new()
	Global.cpp_core = cpp_core
	cpp_core.initialise(%floor_layer_node, %objects_layer_below_node, %player_layer_node, %objects_layer_above_node, %roof_layer_node, %player_basic_overlay, %world_camera)

	var thing = TestEventBus.new()
	var buffer: GodotByteBuffer = GodotByteBuffer.allocate()
	buffer.initialise(Global.cpp_core.get_payload_size())
	buffer.print_out()
	buffer.write_uint32(5)
	thing.send(0, buffer)
	print(thing.channel_ids, thing.bus_ids)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	Global.cpp_core.process(delta)
