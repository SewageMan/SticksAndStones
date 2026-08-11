extends Node2D

@onready var inv_entity = preload("res://objects/Entities/inventory_entity.tscn")
@onready var tile_entity = preload("res://objects/Electrical/tile_entity.tscn")
@onready var cable_entity = preload("res://objects/Electrical/cable_entity.tscn")
@onready var inv: Inv = preload("res://objects/Items/player_inventory.tres")
@onready var heatObj = preload("res://objects/Entities/utility/heat_obj.tscn")
@onready var connectObj = preload("res://MenuStuff/Circuit/popup_menu.tscn")
#var UI = get_node("/root/World/camera2d/overworld_ui")
@onready var lib: InvResource = preload("res://Scripts/worldResourceLogic/ResourceLib.tres")
@onready var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@onready var netlist: Netlist = preload("res://objects/Electrical/netlist.tres")
@onready var info_array: Array = [lib.inv_index, lib.item_index, lib.name_index]
@onready var blocktypes = {
	"hasinventory": [inv_entity, "Inventory_entity"],
	"tile": [tile_entity, "tile_entity"]
}
@export var tileVector = {
	"resistor" : [],
	"ground" : [],
	"battery": []
}
var id: int = 0
var block
var cablePlacing: bool = false
var connectingObj = null


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	Global.playerTiles = self
	set_as_top_level(true)

func _check_occupato(position):
	var occupied: bool = false
	#print("position: ", position, " data: ", WorldSave.load_block_data(position))
	if WorldSave.load_block_data(position):
		for i in range(WorldSave.load_block_data(position).size()):
			if WorldSave.load_block_data(position)[i][2] == $ColorRect.position:
				occupied = true
	if !occupied:
		occupied = check_collision()
	return occupied

func _unhandled_key_input(event: InputEvent) -> void:
	#Debug, this is used to run the compiling process temporarely
	if event.pressed && event.keycode == KEY_G:
		process_connections()

#This is the "compiler" for the connections it makes every seperate black box connect
func process_connections():
	netlist.letlist.clear()
	var elOBJArray: Array
	#var connectingPoints: Array
	var connectingPoints = {}
	for _i in get_children():
		if "tile_entity" in _i.name:
			var elOBJ = _i.get_node("ElectricOBJ")
			elOBJArray.append(elOBJ)
	#This is dedicated to making the output/inputs connected, so that they may be merged later
	#This cycles once through elements, which means they do not register 3+ connections only 2!!
	for _i in elOBJArray:
		if _i.localnetlist.letlist:
			for key in _i.connectedObjects:
				if _i.connectedObjects[key] in elOBJArray:
					var index = elOBJArray.find(_i.connectedObjects[key])
					if elOBJArray[index].localnetlist.letlist:
						if key-2 in elOBJArray[index].orientation ||  key+2 in elOBJArray[index].orientation:
							var keyer: int
							if key-2 > 0:
								keyer = key-2
							if key+2 <= 4:
								keyer = key+2
							#This is super bad but it kinda works so whatever
							var node1
							var node2
							for _j in _i.localnetlist.letlist:
								if ("Input" in _j.type || "Output" in _j.type) && _j.toNode == key:
									node1 = _j.fromNode
							for _j in elOBJArray[index].localnetlist.letlist:
								if ("Input" in _j.type || "Output" in _j.type) && _j.toNode == keyer:
									node2 = _j.fromNode
							connectingPoints[node1] = node2
	print("Connecting this shit together:")
	print(connectingPoints)
	for key in connectingPoints:
		for keyr in connectingPoints:
			if key == connectingPoints[keyr]:
				connectingPoints.erase(key)
	print(connectingPoints)
	for _i in elOBJArray:
		if _i.localnetlist.letlist:
			for _j in _i.localnetlist.letlist:
				if "Input" not in _j.type && "Output" not in _j.type:
					if _j.toNode in connectingPoints:
						_j.toNode = connectingPoints[_j.toNode]
						connectingPoints.erase(_j.toNode)
					if _j.fromNode in connectingPoints:
						_j.fromNode = connectingPoints[_j.fromNode]
						connectingPoints.erase(_j.fromNode)
	#TODO: Make the node numbers sequencial (DONE)
	print("Max nodenumb: ", netlist.maxnodeIndex)
	var nodeDict = {}
	for i in range(elOBJArray.size()):
		if elOBJArray[i].localnetlist.letlist:
			for j in range(elOBJArray[i].localnetlist.letlist.size()):
				if elOBJArray[i].localnetlist.letlist[j].fromNode in nodeDict:
					nodeDict[elOBJArray[i].localnetlist.letlist[j].fromNode].append([i,j, "fromNode"])
				elif elOBJArray[i].localnetlist.letlist[j].fromNode != 0 && elOBJArray[i].localnetlist.letlist[j].toNode != 0:
					nodeDict[elOBJArray[i].localnetlist.letlist[j].fromNode] = [[i,j, "fromNode"]]
				if elOBJArray[i].localnetlist.letlist[j].toNode in nodeDict:
					nodeDict[elOBJArray[i].localnetlist.letlist[j].toNode].append([i,j, "toNode"])
				elif elOBJArray[i].localnetlist.letlist[j].fromNode != 0 && elOBJArray[i].localnetlist.letlist[j].toNode != 0:
					nodeDict[elOBJArray[i].localnetlist.letlist[j].toNode] = [[i,j, "toNode"]]
	print(nodeDict)
	var counter: int = 1
	for key in nodeDict:
		var info = nodeDict[key]
		for _i in info:
			if _i[2] == "fromNode":
				elOBJArray[_i[0]].localnetlist.letlist[_i[1]].fromNode = counter
			else:
				elOBJArray[_i[0]].localnetlist.letlist[_i[1]].toNode = counter
		counter+=1
	netlist.maxnodeIndex = counter
	for _i in elOBJArray:
		if _i.localnetlist.letlist:
			print("NEWEST OBJECT PRINT:")
			for _j in _i.localnetlist.letlist:
				print(_j.type, " ", _j.fromNode, " ", _j.toNode, " ", _j.Value)
	for _i in elOBJArray:
		if _i.localnetlist.letlist:
			for _j in _i.localnetlist.letlist:
				if "Output" not in _j.type && "Input" not in _j.type:
					netlist.letlist.append(_j)
	print("Public netlist:")
	for _i in netlist.letlist:
		print(_i.type, " ", _i.fromNode, " ", _i.toNode, " ", _i.Value)


func place_item(chunk_coords, nam): 
	if Global.objectCount >= id:
		id = Global.objectCount + 1
	WorldSave.save_blocks(chunk_coords, $ColorRect.position, [nam, id, $ColorRect.position, Global.handItem.attributes])
	for _i in Global.handItem.attributes:
		#This code is still really raw and doesn't allow anything other then inv blocks to exist
		#I guess ill look into improving it once we get something other then inv blocks <- lol its still sucks
		if _i in blocktypes and blocktypes[_i]:
			var obj = blocktypes[_i][0].instantiate()
			obj.position = $ColorRect.position
			obj.name = blocktypes[_i][1] + str(id)
			obj.invItem = info_array[1][info_array[2][nam][1]]
			obj.type = nam
			add_child(obj)
			if _i == "hasinventory" && "block" in Global.handItem.attributes:
				var index = info_array[2][nam][1]
				obj.get_child(info_array[2][nam][1]).visible = true
				world_inv.init_inv(info_array[0][index], id)
			if _i == "tile":
				#IMPORTANT! : place the tile attribute under the "placeable" attribute
				var tilename = Global.handItem.attributes[Global.handItem.attributes.find("placeable") + 1]
				var tileLib = {"wire": 0, "resistor": 1, "ground": 3, "battery": 2}
				BetterTerrain.set_cell($TileMapLayer, floor($ColorRect.position/32.0), tileLib[tilename])
				tileVector[tilename].append(floor($ColorRect.position/32.0))
				print(tileVector)
				BetterTerrain.update_terrain_cells($TileMapLayer, tileVector[tilename], true)
				obj.init(Global.handItem)
			if "heat" in Global.handItem.attributes:
				var index = info_array[2][obj.type][1]
				var furnaceID = WorldSave.load_block_id(chunk_coords, global_position)
				if "hasinventory" in Global.handItem.attributes:
					var heat_obj = heatObj.instantiate()
					heat_obj.position = obj.position
					obj.get_node(obj.type).add_child(heat_obj)
					heat_obj.init(index, info_array[1][index])
					heat_obj.name = "heat_OBJ" + str(furnaceID)
					obj.get_node(obj.type).init(index)
					obj.get_node(obj.type).blockID = furnaceID
				if "electric" in Global.handItem.attributes:
					print("electric spawned")
					var heat_obj = heatObj.instantiate()
					heat_obj.position = obj.position
					obj.get_node("ElectricOBJ").add_child(heat_obj)
					heat_obj.init(index, info_array[1][index])
					heat_obj.name = "heat_OBJ" + str(furnaceID)
	#print(id, " ", world_inv.Invent[id])
	#TODO: CHANGE THE ID SYSTEM!! will create problems when the world is exited/entered
	#Will also cause problems, just in general
	Global.objectCount = id
	id += 1
	for j in range(9):
		if inv.slots[j].item:
			#print(inv.slots[j].item.name, " ", Global.handItem.name)
			if inv.slots[j].item.name == Global.handItem.name:
				inv.removeFromInventory(j)
				break

func connect_to(netlist, item):
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var mousePos = get_global_mouse_position()
	var chunk_coords = Vector2(floor(mousePos.x/(16.0*64.0)), floor(mousePos.y/(16.0*64.0)))
	var UI = get_node("/root/World/CanvasLayer/camera2d/Overworld_UI")
	$ColorRect.position = 32*floor(mousePos/32.0)
	#Opening after placement
	if Input.is_action_just_pressed("Click") && !UI.is_open && _check_occupato(chunk_coords):
		if WorldSave.load_block_data(chunk_coords):
			for i in range(WorldSave.load_block_data(chunk_coords).size()):
				if WorldSave.load_block_data(chunk_coords)[i][2] == $ColorRect.position && "hasinventory" in WorldSave.load_block_data(chunk_coords)[i][3]:
					UI._open_inventory(WorldSave.load_block_data(chunk_coords)[i][1], WorldSave.load_block_data(chunk_coords)[i][0])
	#placing
	if Input.is_action_just_pressed("Click") && !UI.is_open && !_check_occupato(chunk_coords):
		if Global.handItem && "placeable" in Global.handItem.attributes:
			place_item(chunk_coords, Global.handItem.name)
	#connecting
	if Input.is_action_just_pressed("Click") && !UI.is_open:
		if Global.handItem && "connectable" in Global.handItem.attributes:
			var player_tiles = get_node_or_null("/root/World/playerTiles").get_children()
			for _i in player_tiles:
				if _i.position == $ColorRect.position && _i != $ColorRect:
					block = _i
			if block and block.get_node("ElectricOBJ").get_node("Popup"):
				print("starting connection", connectingObj)
				connectingObj = !connectingObj
				#cablePlacing[1] = connect_to(netlist, Global.handItem.name)
	#breaking (its different from world related mining)
	if Input.is_action_just_pressed("right_click") && !UI.is_open && _check_occupato(chunk_coords):
		var player_tiles = get_node_or_null("/root/World/playerTiles").get_children()
		#print(world_nodes)
		for _i in player_tiles:
			if _i.position == $ColorRect.position && _i != $ColorRect:
				#print(_i)
				block = _i
	var progressBar = UI.get_child(0).get_child(2)
	progressBar.visible = false
	if block && Input.is_action_pressed("right_click") && !UI.is_open && _check_occupato(chunk_coords) && block.position == $ColorRect.position:
		progressBar.visible = true
		block.minetime -= 0.1
		progressBar.scale.x = block.minetime/15.0
		#print("did this not do anything?: ", block.minetime)
		if block.minetime <= 0:
			var player_tiles = get_node_or_null("/root/World/playerTiles").get_children()
			var index = player_tiles.find(block)
			var blockID: int
			#Getting the item that the object holds back into inventory after breaking.
			for _i in WorldSave.load_block_data(chunk_coords):
				if _i[2] == $ColorRect.position:
					blockID = _i[1]
			for _i in world_inv.get_inv_slots(blockID):
				if _i.item:
					inv.appendInventory(_i.item, _i.amount)
			WorldSave.remove_block_data(chunk_coords, $ColorRect.position)
			inv.appendInventory(player_tiles[index].invItem, 1)
			for _i in player_tiles[index].get_children():
				$TileMapLayer.erase_cell(floor($ColorRect.position/32.0))
				$TileMapLayer.set_cells_terrain_connect([floor($ColorRect.position/32.0)], 0, -1, false)
				for _j in tileVector:
					tileVector[_j].erase(floor($ColorRect.position/32.0))
				print(tileVector)
				_i.queue_free()
			player_tiles[index].queue_free()
			block = null


func check_collision():
	if $ColorRect/ColorRect.has_overlapping_bodies():
		return true
	else:
		return false
