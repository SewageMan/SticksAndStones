extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var cpp_core = CppGodotAdapter.new()
	Global.cpp_core = cpp_core
	cpp_core.initialise(%floor_layer_node, %objects_layer_below_node, %objects_layer_above_node, %roof_layer_node)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	Global.cpp_core.process(delta)
