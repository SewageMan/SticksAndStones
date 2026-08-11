extends Node2D

# form: Key, Input, Output, sprite, showname, option names
var Elements = {
	"Ground": [1, 0, "res://Sprites/Menu/Elements/ground.png", "GND", ["Remove"]],
	#dc source par0: voltage, par1: inner resistance, par2: temp
	"Inputel": [1, 0, "res://Sprites/Menu/Elements/Input.png", "", ["Remove", "Set to top", "Set to left", "Set to bottom", "Set to right"]],
	"Outputel": [1,0,"res://Sprites/Menu/Elements/output.png", "", ["Remove", "Set to top", "Set to left", "Set to bottom", "Set to right"]],
	"CurrentSource": [1,1, "res://Sprites/Menu/Elements/CurrentSource.png", "I", ["Remove", "Set current"]],
	"DCSource": [1, 1, "res://Sprites/Menu/Elements/DCSource.png", "V", ["Remove", "Set voltage"]],
	#resistor par0: init_resistance, temperature
	"Resistor": [1, 1, "res://Sprites/Menu/Elements/Resistor.png", "R", ["Remove", "Set resitance"]],
	"Diode": [1,1, "res://Sprites/Menu/Elements/diode.png", "D", ["Remove", "Set static semiconductor voltage", "Set breakdown voltage"]],
	"Thyristor": [2, 1, "res://Sprites/Menu/Elements/Thyristor.png", "SCR"]
}
var type: String
var nodes: Array
var branch: int
var holding: bool = false
var inverted: bool = false
signal makeLine2D
signal destroyLine2D
signal updateLine2D
@export var value: Vector2
@onready var popup = preload("res://MenuStuff/Circuit/popup_menu.tscn")

func init(element):
	type = element
	var positionAnchor
	var inputPositions: Array
	var outputPositions: Array
	for _i in $Sprite2D.get_children():
		if element in _i.name:
			positionAnchor = _i
	for _i in positionAnchor.get_children():
		if "Input" in _i.name:
			inputPositions.append(_i.position)
	for _i in positionAnchor.get_children():
		if "Output" in _i.name:
			outputPositions.append(_i.position)
	print(inputPositions, outputPositions)
	$Sprite2D/InputHolder.CreateInputs(Elements[element][0], inputPositions, self)
	for _i in $Sprite2D/InputHolder.get_children():
		nodes.append(_i)
	$Sprite2D/OutputHolder.CreateOutputs(Elements[element][1], outputPositions, self)
	for _i in $Sprite2D/OutputHolder.get_children():
		nodes.append(_i)
	var infopos = positionAnchor.get_node("InfoHandler").position
	$Sprite2D.texture = load(Elements[element][2])
	$Sprite2D/Info.get_child(0).text = Elements[element][3]
	$Sprite2D/Info.position = infopos

func processInputClick():
	#accounting for rotation, movement from origin and inversion
	var invVect: Vector2
	if inverted:
		invVect = Vector2(((-1)*float(inverted) + 1*float(!inverted)),1).rotated(2*self.rotation)
	else:
		invVect = Vector2(1,1)
	Global.temp_pos[0] = Global.temp_pos[0].rotated(self.rotation)*invVect + self.position
	makeLine2D.emit()

func processOutputClick():
	#accounting for rotation, movement from origin and inversion
	var invVect: Vector2
	if inverted:
		invVect = Vector2(((-1)*float(inverted) + 1*float(!inverted)),1).rotated(2*self.rotation)
	else:
		invVect = Vector2(1,1)
	Global.temp_pos[0] = Global.temp_pos[0].rotated(self.rotation)*invVect + self.position
	makeLine2D.emit()

func remConnections():
	destroyLine2D.emit()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	%InputHolder.clickInput.connect(processInputClick)
	%OutputHolder.clickOutput.connect(processOutputClick)
	%InputHolder.remconnection.connect(remConnections)
	%OutputHolder.remconnection.connect(remConnections)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var mpos = get_global_mouse_position() - Global.panelPos - Global.camera.position
	if holding:
		position = mpos
	self.scale = Vector2((-1)*float(inverted) + 1*float(!inverted), 1)

func _unhandled_key_input(event: InputEvent) -> void:
	if event.pressed && event.keycode == KEY_R && holding:
		self.rotation += PI/2
	if event.pressed && event.keycode == KEY_F && holding:
		inverted = !inverted

func _on_area_2d_input_event(viewport: Node, event: InputEvent, shape_idx: int) -> void:
	if event is InputEventMouseButton && event.pressed && event.button_index == MOUSE_BUTTON_LEFT && !Input.is_action_pressed("sprint"):
		if get_node("Poppup"):
			get_node("Poppup").queue_free()
		holding = true
		for _i in nodes:
			#false makes sure the line2D gets invisibled instead of deleted
			Global.temp_pos = [_i.position, _i, false]
			destroyLine2D.emit()
	if event is InputEventMouseButton && !event.pressed && event.button_index == MOUSE_BUTTON_LEFT && !Input.is_action_pressed("sprint"):
		holding = false
		position = 64*floor((get_global_mouse_position()-Global.panelPos-Global.camera.position)/64.0)+Vector2(32,32)
		#passing movement, rotation and inversion
		var invVect: Vector2
		if inverted:
			invVect = Vector2(((-1)*float(inverted) + 1*float(!inverted)),1).rotated(2*self.rotation)
		else:
			invVect = Vector2(1,1)
		Global.temp_pos = [self.position, nodes, self.rotation, invVect]
		if nodes.size()>0:
			updateLine2D.emit()
	if event is InputEventMouseButton && event.pressed && event.button_index == MOUSE_BUTTON_RIGHT:
		var popupmenu = popup.instantiate()
		popupmenu.name = "Poppup"
		popupmenu.init(Elements[type][4])
		add_child(popupmenu)

func _on_area_2d_mouse_entered() -> void:
	pass
