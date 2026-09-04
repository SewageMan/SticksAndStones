extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var cpp_core = CppGodotAdapter.new()
	Global.cpp_core = cpp_core
	cpp_core.initialise(%floor_layer_node, %objects_layer_below_node, %player_layer_node, %objects_layer_above_node, %roof_layer_node, %player_basic_overlay, %world_camera)

	%Player.on_cpp_ready()

	var buffer: GodotByteBuffer = GodotByteBuffer.allocate()

	#var thing = TestEventBus.new()
	#buffer.initialise(Global.cpp_core.get_payload_size())
	#buffer.print_out()
	#buffer.write_uint32(5)
	#thing.send(0, buffer)
	#print(thing.channel_ids, thing.bus_ids)

	buffer.clear()
	buffer.initialise()
	buffer.write_string("whenametuinasama")

	var buffer2: GodotByteBuffer = GodotByteBuffer.allocate()
	buffer2.initialise(16)
	buffer2.write_buffer(buffer)

	buffer2.set_offset(0)
	var buffer3: GodotByteBuffer = buffer2.read_buffer()

	var res: String = buffer3.read_string()

	print(res)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	Global.cpp_core.process(delta)
