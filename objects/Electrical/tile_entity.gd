extends Node2D

@export var type = null
@export var tileType: String
@export var invItem = null
@export var minetime: float = 1.5
@onready var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@onready var electric = preload("res://objects/Electrical/electric_obj.tscn")
@onready var inv = preload("res://objects/Entities/inventory_entity.tscn")
@onready var lib: InvResource = preload("res://Scripts/worldResourceLogic/ResourceLib.tres")
@onready var info_array: Array = [lib.inv_index, lib.item_index, lib.name_index]

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func init(item):
	var attributes = item.attributes
	if "electric" in attributes:
		var electricObj = electric.instantiate()
		electricObj.name = "ElectricOBJ"
		electricObj.position += Vector2(16,16)
		var index = attributes.find("placeable") + 1
		var diffpos = self.global_position - Global.camera.position
		var direction = floor(2*(diffpos.angle()+PI)/PI + 0.1*PI) 
		electricObj.init(attributes[index], item, direction)
		add_child(electricObj)
	if "hasinventory" in attributes:
		var invOBJ = inv.instantiate()
		invOBJ.name = "tile_inv"
		invOBJ.type = item.name
		world_inv.init_inv(info_array[0][info_array[2][item.name][0]], Global.objectCount)
		add_child(invOBJ)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
