extends Camera2D

var moveAround: bool
var oldMouse: Vector2

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton && event.is_pressed() && event.button_index == MOUSE_BUTTON_MIDDLE:
		moveAround = true
		oldMouse = get_global_mouse_position()
	if event is InputEventMouseButton && !event.is_pressed() && event.button_index == MOUSE_BUTTON_MIDDLE:
		moveAround = false

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#print(moveAround)
	if moveAround:
		self.position -= get_global_mouse_position() - oldMouse
