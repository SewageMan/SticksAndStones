extends Control

@onready var chest_inventory: Array = $NinePatchRect/VBoxContainer/GridContainer.get_children()
@onready var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")


#TODO: REdoo this why did i think this was a good idea, brainrot!
#this closes! Closes, future me, gets culled out of the scene! Closes! none of this exists
#once you close the menu, DO NOT have burning logic here

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	%TurnFurnaceON.init("Turn Furnace ON/OFF", true)
	%Control.init("Emmit external signal", true)

func unpressButton():
	%TurnFurnaceON.force_unclick()
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
