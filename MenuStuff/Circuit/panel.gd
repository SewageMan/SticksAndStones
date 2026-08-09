extends Node2D

@onready var circuit_obj = preload("res://objects/Elements/Circuit.gd")
@onready var element = preload("res://objects/Elements/element.tscn")
@onready var line = preload("res://objects/Elements/connect_wire.tscn")
@onready var netlist: Netlist = preload("res://objects/Electrical/netlist.tres")
var lineobj
var conThing: bool = false
var pointPair: Array
var wireIndex: int = 0
var circuitIndex: int = 0
var moveAround: bool
var oldMouse: Vector2
@export var elementArray: Array
@export var connectionPairs: Array
@export var nodes: Array
@export var lineArray: Array
#this is for creating paths

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	
	#this is just debug stuff
	var obj = element.instantiate()
	##adding DCSource
	#obj.init("DCSource")
	#obj.position += Vector2(32,160)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##adding thyristor
	#obj = element.instantiate()
	#obj.init("Resistor")
	#obj.position += Vector2(288,160)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##another one
	#obj = element.instantiate()
	#obj.init("Resistor")
	#obj.position += Vector2(288*2,160)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##another
	#obj = element.instantiate()
	#obj.init("Resistor")
	#obj.position += Vector2(288,160+64)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##another
	#obj = element.instantiate()
	#obj.init("Resistor")
	#obj.position += Vector2(288,160+32)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	#another
	obj = element.instantiate()
	obj.init("Resistor")
	obj.position += Vector2(288,160)
	obj.makeLine2D.connect(line2DConnector)
	obj.destroyLine2D.connect(line2DDestroyer)
	obj.updateLine2D.connect(line2DUpdater)
	elementArray.append(obj)
	add_child(obj)
	#another
	obj = element.instantiate()
	obj.init("Resistor")
	obj.position += Vector2(288,160-32)
	obj.makeLine2D.connect(line2DConnector)
	obj.destroyLine2D.connect(line2DDestroyer)
	obj.updateLine2D.connect(line2DUpdater)
	elementArray.append(obj)
	add_child(obj)
	##another
	#obj = element.instantiate()
	#obj.init("Ground")
	#obj.position += Vector2(288,288+64)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##another
	#obj = element.instantiate()
	#obj.init("Ground")
	#obj.position += Vector2(288-64,288+64)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	##another
	#obj = element.instantiate()
	#obj.init("Outputel")
	#obj.position += Vector2(288*2,288+64)
	#obj.makeLine2D.connect(line2DConnector)
	#obj.destroyLine2D.connect(line2DDestroyer)
	#obj.updateLine2D.connect(line2DUpdater)
	#elementArray.append(obj)
	#add_child(obj)
	#another
	obj = element.instantiate()
	obj.init("Outputel")
	obj.position += Vector2(288*2,288+32)
	obj.makeLine2D.connect(line2DConnector)
	obj.destroyLine2D.connect(line2DDestroyer)
	obj.updateLine2D.connect(line2DUpdater)
	elementArray.append(obj)
	add_child(obj)
	#another
	obj = element.instantiate()
	obj.init("Inputel")
	obj.position += Vector2(32,288+32)
	obj.makeLine2D.connect(line2DConnector)
	obj.destroyLine2D.connect(line2DDestroyer)
	obj.updateLine2D.connect(line2DUpdater)
	elementArray.append(obj)
	add_child(obj)
	#another
	obj = element.instantiate()
	obj.init("Inputel")
	obj.position += Vector2(32,288+64)
	obj.makeLine2D.connect(line2DConnector)
	obj.destroyLine2D.connect(line2DDestroyer)
	obj.updateLine2D.connect(line2DUpdater)
	elementArray.append(obj)
	add_child(obj)
	for i in range(elementArray.size()):
		elementArray[i].name = "Element_"+str(circuitIndex)+"_"+str(i)
		elementArray[i].branch = i
		elementArray[i].get_child(0).get_node("Info").get_child(0).text += str(circuitIndex)+"_"+str(i)

func line2DConnector():
	var tpos = Global.temp_pos[0]
	if !conThing:
		for i in range(3):
			$wire_visual.add_point(tpos)
		lineobj = line.instantiate()
		lineobj.add_point(tpos)
		lineobj.pointData[Global.temp_pos[1]] = 0
		pointPair.append(Global.temp_pos[1])
		conThing = true
	else:
		lineobj.add_point(tpos)
		lineobj.pointData[Global.temp_pos[1]] = 2
		pointPair.append(Global.temp_pos[1])
		connectionPairs.append(pointPair)
		pointPair = []
		lineobj.name += "_"+str(wireIndex)
		add_child(lineobj)
		lineArray.append(lineobj)
		wireIndex+=1
		lineobj = null
		$wire_visual.clear_points()
		conThing = false

func line2DDestroyer():
	print("got this far!")
	if Global.temp_pos[2]:
		#removing set line visuals
		for _i in lineArray:
			if Global.temp_pos[1] in _i.pointData:
				_i.queue_free()
				lineArray.erase(_i)
		#erasing pairs from array
		for _i in connectionPairs:
			if Global.temp_pos[1] in _i:
				connectionPairs.erase(_i)
	else:
		#making invisible line visuals
		for _i in lineArray:
			if Global.temp_pos[1] in _i.pointData:
				_i.visible = false

func line2DUpdater():
	for _i in self.get_children():
		if "wire_connect" in _i.name:
			for _j in Global.temp_pos[1]:
					if _j in _i.pointData:
						#setting effected point position to the new rotated moved and inverted position
						_i.set_point_position(_i.pointData[_j], _j.position.rotated(Global.temp_pos[2])*Global.temp_pos[3] + Global.temp_pos[0])
						_i.recStuff(0)
						_i.visible = true

func compile():
	#connection check
	var tempArray: Array
	var has_connections: bool = true
	for _i in elementArray:
		for _j in _i.get_child(0).get_node("InputHolder").get_children():
			tempArray.append(_j)
		for _j in _i.get_child(0).get_node("OutputHolder").get_children():
			tempArray.append(_j)
	for _i in tempArray:
		if _i.hasBeenClicked == false:
			has_connections = false
	if has_connections:
		#Creating node connections,
		#Code creddits to Igor:
		var a = connectionPairs
		var c = {}
		var d = []
		var b = []
		for i in a:
			var n1 = i[0]
			var n2 = i[1]
			if n1 not in c.keys() and n2 not in c.keys():
				d.append([n1,n2])
				c[n1]=len(d)-1
				c[n2]=len(d)-1
			elif n1 in c and n2 in c:
				if c[n1] == c[n2]:
					print("double connection")
				else:
					for k in d[c[n1]]:
						c[k] = c[n1]
					d[c[n1]]+=d[c[n2]]
					d[c[n2]] = null
			else:
				if n1 not in c.keys():
					c[n1]=c[n2]
					d[c[n2]].append(n1)
				else:
					c[n2] = c[n1]
					d[c[n1]].append(n2)
		for i in d:
			if i != null:
				b.append(i)
		#combining all ground connections and making it first:
		var e = []
		for _i in b:
			for _k in _i:
				if "Ground" in _k.belongingElement.type:
					for _j in _i:
						if _j.belongingElement.type != "Ground":
							e.append(_j)
		nodes.append(e)
		print("Before ground detection: ", b)
		for _i in e:
			for _j in b:
				if _i in _j:
					b.erase(_j)
		for _i in b:
			nodes.append(_i)
		print("Only ground: ", e)
		print("Result: ", nodes)
		#Creating branch for each element and filling the netlist
		var maxnode = netlist.maxnodeIndex
		var nextmax = maxnode
		for i in range(elementArray.size()):
			var branch_obj = Branch.new()
			for _j in elementArray[i].nodes:
				for k in range(nodes.size()):
					if _j in nodes[k]:
						branch_obj.type = elementArray[i].type
						branch_obj.Value = elementArray[i].value
						var isGround: bool = true
						if elementArray[i].type == "Ground":
							isGround = false
						if "Input" in _j.name:
							branch_obj.fromNode = (k + maxnode) * int(isGround)
						if "Output" in _j.name:
							branch_obj.toNode = (k + maxnode) * int(isGround)
						if nextmax < (k+maxnode):
							nextmax = (k+maxnode)
			if branch_obj.type:
				netlist.letlist.append(branch_obj)
		netlist.maxnodeIndex = nextmax
		for _i in netlist.letlist:
			print(_i.type, " ", _i.fromNode, " ", _i.toNode, " ", _i.Value)
		print("compiled!", " Maxnode: ", nextmax)
	else:
		print("Circuit is open!")
	var circuit_save = circuit_obj.new()
	circuit_save.index = circuitIndex
	circuit_save.connection_array = connectionPairs
	circuit_save.elements = elementArray
	for _i in elementArray:
		circuit_save.elements_pos.append(_i.position)
	WorldSave.save_circuit(circuitIndex, circuit_save)

func _unhandled_key_input(event: InputEvent) -> void:
	#Debug, this func will usually run when compiling the blackbox
	if event.pressed && event.keycode == KEY_L:
		compile()


func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton && event.is_pressed() && event.button_index == MOUSE_BUTTON_MIDDLE:
		moveAround = true
		oldMouse = get_global_mouse_position()-self.position
	if event is InputEventMouseButton && !event.is_pressed() && event.button_index == MOUSE_BUTTON_MIDDLE:
		moveAround = false




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#removes visual wire
	#print(get_global_mouse_position())
	if moveAround:
		self.position += (get_global_mouse_position()-(self.position + oldMouse))/10.0
	Global.panelPos = self.position
	if Input.is_action_just_pressed("right_click") && conThing:
		conThing = false
		pointPair = []
		lineobj = null
		$wire_visual.clear_points()
