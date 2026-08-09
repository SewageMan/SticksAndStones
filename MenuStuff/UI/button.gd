extends Control
var areaHasBeenEntered: bool = false
var pushed_in: bool
@onready var startPos: Vector2 = %Label.position

signal pressed

#bool false is push-button, true is for toggle
func init(buttontext: String, type: bool):
	%Label.text = buttontext
	var length = buttontext.length()
	$Area2D.scale.x = length*7.5
	pushed_in = type

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	init("Turn furnace ON", true)


func _on_area_2d_body_entered(body: Node2D) -> void:
	if self.visible:
		areaHasBeenEntered = true
		#print(self.name)
		#print(body.name)
		#print("ENTERED")
	else:
		areaHasBeenEntered = false

func _on_area_2d_body_exited(body: Node2D) -> void:
	if self.visible:
		areaHasBeenEntered = false
		#print(self.name)
		#print("EXITED")

func force_unclick():
	if pushed_in:
		%buttonPressed.visible = false
		%buttonDefault.visible = true
		%Label.position.y = startPos.y
		pressed.emit()

func _process(delta):
	if Input.is_action_just_pressed("Click") && areaHasBeenEntered:
		%buttonPressed.visible = !%buttonPressed.visible
		%buttonDefault.visible = !%buttonDefault.visible
		if %buttonPressed.visible == true:
			%Label.position.y = startPos.y + 5
		if %buttonPressed.visible == false:
			%Label.position.y = startPos.y
		pressed.emit()
	if Input.is_action_just_released("Click") && !pushed_in:
		%buttonPressed.visible = false
		%buttonDefault.visible = true
		%Label.position.y = startPos.y
		pressed.emit()
