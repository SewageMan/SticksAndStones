extends Control

@export var inventory_id: int
@onready var chest_inventory: Array = $chest_inventory/NinePatchRect/GridContainer.get_children()
@onready var furnace_inventory: Array = $furnace_inventory/NinePatchRect/VBoxContainer/GridContainer.get_children()
@onready var kiln_inventory: Array = $kiln_inventory/NinePatchRect/VBoxContainer/GridContainer.get_children()
@onready var invLib = [chest_inventory, furnace_inventory, kiln_inventory]
@onready var buttonLib = [%TurnFurnaceON, %Control]
@onready var returnValue: Array
var index

#TODO: POV: you just opened this and wondering what i was doing: i was connecting a button press to
#the activation of the heat object

func _ready() -> void:
	furnace_inventory.append($furnace_inventory/NinePatchRect/VBoxContainer.get_child(0))
	kiln_inventory.append($kiln_inventory/NinePatchRect/VBoxContainer.get_child(0))

func init(type):
	index = invLib[type]
	var inventories = self.get_children()
	inventories[type].visible = true

func unpressHeatButtons():
	self.get_child(1).unpressButton()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#All temperature scales
	if returnValue:
		%Temperature.value = returnValue[0] / 13.0
		%BurnTime.text = "Burn Time: " + str(floor(returnValue[1]*10)/10)
		%Temperature2.text = "Temp: " + str(floor(returnValue[0])) + "°C"
