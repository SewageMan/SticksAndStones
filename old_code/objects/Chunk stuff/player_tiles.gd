extends Node2D

@onready var inv_entity = preload("res://objects/Entities/inventory_entity.tscn")
@onready var inv: Inv = preload("res://objects/Items/player_inventory.tres")
#var UI = get_node("/root/World/camera2d/overworld_ui")
@onready var lib: InvResource = preload("res://Scripts/worldResourceLogic/ResourceLib.tres")
@onready var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@onready var info_array: Array = [lib.inv_index, lib.item_index, lib.name_index]
var id: int = 0
var block

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	set_as_top_level(true)

func _check_occupato(position):
	var occupied: bool = false
	print("position: ", position, " data: ", WorldSave.load_block_data(position))
	if WorldSave.load_block_data(position):
		for i in range(WorldSave.load_block_data(position).size()):
			if WorldSave.load_block_data(position)[i][2] == $ColorRect.position:
				occupied = true
	return occupied

func place_item(chunk_coords, nam):
	WorldSave.save_blocks(chunk_coords, $ColorRect.position, [nam, id, $ColorRect.position])
	var obj = inv_entity.instantiate()
	obj.invItem = info_array[1][info_array[2].find(nam)]
	obj.name = "Inventory_entity_" + str(id)
	obj.position = $ColorRect.position
	obj.type = nam
	add_child(obj)
	var index = info_array[2].find(nam)
	world_inv.init_inv(info_array[0][index], id)
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
	#Visuals update, by hand, yeah i know it sucks but you try to figure out how to do this better.
	if nam == "Stone Furnace":
		obj.get_child(0).visible = false
		obj.get_child(1).visible = true

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
				if WorldSave.load_block_data(chunk_coords)[i][2] == $ColorRect.position:
					UI._open_inventory(WorldSave.load_block_data(chunk_coords)[i][1], WorldSave.load_block_data(chunk_coords)[i][0])
	#placing
	if Input.is_action_just_pressed("Click") && !UI.is_open && !_check_occupato(chunk_coords):
		if Global.handItem && Global.handItem.placeable && Global.handItem.name == "Chest":
			place_item(chunk_coords, "Chest")
		if Global.handItem && Global.handItem.placeable && Global.handItem.name == "Stone Furnace":
			place_item(chunk_coords, "Stone Furnace")
	#breaking (its different from world related mining
	if Input.is_action_just_pressed("right_click") && !UI.is_open && _check_occupato(chunk_coords):
		var player_tiles = get_node_or_null("/root/World/playerTiles").get_children()
		#print(world_nodes)
		for _i in player_tiles:
			if "Inventory_entity" in _i.name && _i.position == $ColorRect.position:
				block = _i
	var progressBar = UI.get_child(0).get_child(2)
	progressBar.visible = false
	if block && Input.is_action_pressed("right_click") && !UI.is_open && _check_occupato(chunk_coords):
		progressBar.visible = true
		block.minetime -= 0.1
		progressBar.scale.x = block.minetime/15.0
		#print("did this not do anything?: ", block.minetime)
		if block.minetime <= 0:
			var player_tiles = get_node_or_null("/root/World/playerTiles").get_children()
			var index = player_tiles.find(block)
			WorldSave.remove_block_data(chunk_coords, $ColorRect.position)
			inv.appendInventory(player_tiles[index].invItem, 1)
			for _i in player_tiles[index].get_children():
				_i.queue_free()
			player_tiles[index].queue_free()
			block = null
