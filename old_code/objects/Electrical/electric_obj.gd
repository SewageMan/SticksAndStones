extends StaticBody2D

@export var circuit: Array
@export var circuitIndex: int
@export var localnetlist = Netlist.new()
@export var orientation: Array[int]
@export var connectedObjects = {}
@export var type: String
@export var dir: int
@onready var netlist: Netlist = preload("res://objects/Electrical/netlist.tres")
@onready var popup = preload("res://MenuStuff/Circuit/popup_menu.tscn")
var selfItem

@export var elements = {
	"resistor": [["Resistor", 1, 2, Vector2(0,0)],
				["Outputel", 2, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]],
	"battery": [["DCSource", 1, 2, Vector2(0,0)],
				["Resistor", 2, 3, Vector2(0,0)],
				["Outputel", 3, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]],
	"cross":   [["Resistor", 3, 5, Vector2(0,0)],
				["Resistor", 3, 4, Vector2(0,0)],
				["Resistor", 2, 3, Vector2(0,0)],
				["Resistor", 1, 3, Vector2(0,0)],
				["Outputel", 5, -1, Vector2(0,0)],
				["Outputel", 4, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 2, -1, Vector2(0,0)]],
	"triple":  [["Resistor", 3, 2, Vector2(0,0)],
				["Resistor", 1, 2, Vector2(0,0)],
				["Outputel", 2, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 3, -1, Vector2(0,0)]],
	"ground":  [[["Resistor", 1, 0, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]],
				[["Resistor", 1, 0, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]],
				[["Resistor", 1, 0, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]],
				[["Resistor", 1, 0, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)],
				["Inputel", 1, -1, Vector2(0,0)]]],
}
var additiveResistance = {"copper" : 0, "iron" : 0.21, "silver": -0.003, "gold": 0.052, "aluminuim": 0.06, "coal": 20}
#resistivity (at 23 T) = [Ohm*m]
var resistivity = {"copper": 1.68e-8, "iron": 9.71e-8, "silver": 1.58e-8, "gold": 2.2e-8, "aluminum": 2.65e-8, "coal": 3.5e-5}
var metalNobility = {"grahpite": 0.1, "gold": 0.05, "copper": -0.35, "silver": -0.15, "aluminum": -0.85, "magnesium": -1.6, "zinc": -1}
var popupSize: int

func init(info, item, placement):
	type = info
	selfItem = item
	dir = placement
	init_netlistInfo()

func openPopup(info):
	if get_node("Popup"):
		get_node("Popup").queue_free()
	else:
		var connectionPopup = popup.instantiate()
		popupSize = len(info)
		connectionPopup.initEOBJ(info)
		connectionPopup.name = "Popup"
		add_child(connectionPopup)
		connectionPopup.update(0)

func closePopup():
	if get_node("Popup"):
		get_node("Popup").queue_free()

func init_netlistInfo():
	localnetlist.letlist.clear()
	var netlistArray: Array
	var elementType: String
	if type == "wire":
		var resistance = selfItem.defValues["resistance"]
		resistance += additiveResistance[selfItem.defValues["material"]]
		var sizestuff = {2 : "resistor", 3: "triple", 4: "cross"}
		if orientation.size() in sizestuff:
			elementType = sizestuff[orientation.size()]
			netlistArray = elements[elementType]
			var count: int = 0
			for _i in netlistArray:
				if _i[2] == -1:
					_i[2] = orientation[count]
					count += 1
		if netlistArray:
			setNetlist(netlistArray, resistance)
	if type == "ground":
		var resistance = 5
		if orientation.size() >= 1:
			netlistArray = elements["ground"][orientation.size()-1]
			var count: int = 0
			for _i in netlistArray:
				if _i[2] == -1:
					print("THIS RAN!!!", count, orientation.size()-1)
					_i[2] = orientation[count]
					count += 1
		if netlistArray:
			setNetlist(netlistArray, resistance)
	if type == "resistor":
		var resistance = selfItem.defValues["resistance"]
		#TODO: Add stuff here to take from the inventory entity that you can stuff with resistive material
		if orientation.size() == 2:
			elementType = "resistor"
			netlistArray = elements[elementType]
			var count: int = 0
			for _i in netlistArray:
				if _i[2] == -1:
					_i[2] = orientation[count]
					count += 1
		if netlistArray:
			setNetlist(netlistArray, resistance)
	if type == "battery":
		pass


func setNetlist(list, value):
	for _i in list:
		if _i[0] == "Resistor":
			_i[3] = Vector2(value, 0)
		if _i[1] > 0:
			_i[1] += netlist.maxnodeIndex
		if _i[2] > 0 and "Output" not in _i[0] and "Input" not in _i[0]:
			_i[2] += netlist.maxnodeIndex
		var branch = Branch.new()
		branch.type = _i[0]
		branch.fromNode = _i[1]
		branch.toNode = _i[2]
		branch.Value = _i[3]
		localnetlist.letlist.append(branch)
	for _i in list:
		for _j in _i:
			if _j is int && _j > netlist.maxnodeIndex:
				netlist.maxnodeIndex = _j
	print("////////// SEPERATION ///////////////")
	for _i in localnetlist.letlist:
		print(_i.type, " ", _i.fromNode, " ", _i.toNode, " ", _i.Value)
	print(netlist.maxnodeIndex)
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	$connection.parentObject = self

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func _on_connection_area_entered(area: Area2D) -> void:
	if area.name == "connection":
		var diffpos =  $connection.global_position - area.global_position
		diffpos = diffpos.normalized()
		var diffangle: int = floor(2*(diffpos.angle()+PI)/PI + 0.1*PI)
		orientation.append(diffangle)
		connectedObjects[diffangle] = area.parentObject
		print("found a simular connection, vector: ", diffpos, " diff angle array: ", orientation)
		init_netlistInfo()
	if area.name == "ColorRect" && Global.handItem && "cable" in Global.handItem.attributes:
		var word: Array
		if localnetlist.letlist:
			for _i in localnetlist.letlist:
				if "Output" in _i.type or "Input" in _i.type:
					word.append(_i.type.left(len(_i.type)-2)+ " Node: (" + str(_i.toNode)+ ")")
		else:
			for _i in elements[type]:
				if "Output" in _i[0] or "Input" in _i[0]:
					word.append(_i[0].left(len(_i[0])-2) + " Node: (" + str(_i[1])+ ")")
		openPopup(word)


func _on_connection_area_exited(area: Area2D) -> void:
	if area.name == "connection":
		var diffpos =  $connection.global_position - area.global_position
		diffpos = diffpos.normalized()
		var diffangle: int = floor(2*(diffpos.angle()+PI)/PI + 0.1*PI)
		orientation.erase(diffangle)
		connectedObjects.erase(diffangle)
		print("removed a simular connection, vector: ", diffpos, " diff angle array: ", orientation)
		init_netlistInfo()
	if area.name == "ColorRect":
		closePopup()

var iter: int = 0
func _unhandled_key_input(event: InputEvent) -> void:
	if Input.is_action_pressed("R") and get_node("Popup"):
		iter += 1
		get_node("Popup").update(iter%popupSize)
