extends Control
@onready var patch = NinePatchRect.new()
var mode: String


func init(info):
	mode = "circuit"
	for _i in info:
		var btn = Button.new()
		btn.text = _i
		%VBoxContainer.add_child(btn)

func initEOBJ(info):
	mode = "elOBJ"
	%EOBJ_panel.visible = true
	for _i in info:
		var str = RichTextLabel.new()
		str.bbcode_enabled = true
		str.fit_content = true
		str.autowrap_mode = TextServer.AUTOWRAP_OFF
		str.append_text(_i)
		str.name = _i
		#str.push_bgcolor(Color.BLUE)
		%VBoxContainer.add_child(str)

func update(selection):
	var selected = %VBoxContainer.get_children()[selection]
	for _i in %VBoxContainer.get_children():
		_i.text = _i.name
	selected.text = ""
	selected.push_bgcolor(Color.BLUE)
	selected.append_text(selected.name)
	selected.pop()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
