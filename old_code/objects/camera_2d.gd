extends Node2D

var maxvelocity
#var m_pos = get_viewport().get_global_mouse_position()
# Called when the node enters the scene tree for the first time.
func _ready():
	Global.camera = self
	set_as_top_level(true)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float):
	position = Global.player.position
	if Input.is_action_just_released("zoom_out"):
		$Camera2D.zoom.x -= 0.05
		$Camera2D.zoom.y -= 0.05
	if Input.is_action_just_released("zoom_in"):
		$Camera2D.zoom.x += 0.05
		$Camera2D.zoom.y += 0.05
