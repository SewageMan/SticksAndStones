extends ColorRect


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Global.camera:
		self.position = 64*floor((get_global_mouse_position()-Global.panelPos-Global.camera.position)/64.0)
