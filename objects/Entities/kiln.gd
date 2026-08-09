extends Node2D

@onready var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@export var blockID: int
var OBJ
var ID: int
var isActive: bool = false
var burning: bool = false
var connected: bool = false

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func init(id):
	ID = id
	for _i in self.get_children():
		if "heat_OBJ" in _i.name:
			OBJ = _i

func toggleActivation():
	isActive = !isActive
	OBJ.setActivation(isActive)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var UI = Global.currentlyOpenUI
	


func _on_smelt_timer_timeout() -> void:
	print("timeout!")
	burning = false
