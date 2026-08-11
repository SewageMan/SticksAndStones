extends Node2D

@export var isMouseOver: bool
@export var connectedLines: Array
@export var hasBeenClicked: bool = false
@export var belongingElement: Node2D

signal clicked
signal removed

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_area_2d_mouse_entered() -> void:
	print("entered!")
	isMouseOver = true


func _on_area_2d_mouse_exited() -> void:
	print("exited!")
	isMouseOver = false


func _on_area_2d_input_event(viewport: Node, event: InputEvent, shape_idx: int) -> void:
	if event is InputEventMouseButton && event.pressed && event.button_index == MOUSE_BUTTON_LEFT && Input.is_action_pressed("sprint"):
			print("hazbeenklicked")
			Global.temp_pos = [self.position, self]
			hasBeenClicked = true
			clicked.emit()
	if event is InputEventMouseButton && event.pressed && event.button_index == MOUSE_BUTTON_RIGHT && Input.is_action_pressed("sprint") && hasBeenClicked:
		print("remclicked")
		#True makes sure that the line gets removed entirely
		Global.temp_pos = [self.position, self, true]
		hasBeenClicked = false
		removed.emit()
