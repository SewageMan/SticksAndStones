extends Node2D

@onready var type: String
@onready var heatObj = preload("res://objects/Entities/utility/heat_obj.tscn")
@onready var lib: InvResource = preload("res://Scripts/worldResourceLogic/ResourceLib.tres")
@onready var info_array: Array = [lib.inv_index, lib.item_index, lib.name_index]
@export var minetime: float = 5.0
@export var invItem: InvItem

signal interactable

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	interactable.emit()
		
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func connectHeatObj():
	for _i in self.get_children():
		if "heat_OBJ" in _i.name:
			_i.initConnection()

func _on_area_2d_body_entered(body: Node2D) -> void:
	pass # Replace with function body.
