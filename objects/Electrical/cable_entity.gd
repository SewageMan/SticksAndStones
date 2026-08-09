extends Node2D
@export var type = null
@export var tileType: String
@export var invItem = null
@export var minetime: float = 1.5
@onready var electric = preload("res://objects/Electrical/electric_obj.tscn")

func init(item, objects):
	var attributes = item.attributes
	var electricObj = electric.instantiate()
	electricObj.name = "ElectricOBJ"
	electricObj.position += Vector2(16,16) 
	electricObj.init(attributes["cable"], item, 0)
	add_child(electricObj)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
