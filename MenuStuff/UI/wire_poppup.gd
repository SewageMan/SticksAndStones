extends Control

func init(info):
	for _i in info:
		var btn = Button.new()
		btn.text = _i
		$VBoxContainer.add_child(btn)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
