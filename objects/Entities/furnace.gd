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
	var objItem
	if OBJ:
		objItem = OBJ.selfItem
	var UI = Global.currentlyOpenUI
	if isActive:
			burn_fuel()
			#print("burning state: ", burning)
			if !burning:
				if UI && is_instance_valid(UI) && UI.inventory_id == blockID:
					UI.unpressHeatButtons()
				isActive = false
				OBJ.setActivation(false)
	#Connect UI button elements to the furnace (buttons and text)
	if UI && is_instance_valid(UI) && UI.inventory_id == blockID && self.visible:
		UI.returnValue = [OBJ.temperature, $BurnTimer.time_left]
		var furnace_inventory = world_inv.get_inv_slots(blockID)
		if furnace_inventory[0].item && "insulation" in furnace_inventory[0].item.attributes:
			OBJ.HTCoeff = objItem.defValues["heatTransCoeff"] - furnace_inventory[0].amount/furnace_inventory[0].item.defValues["insulation"]
		if !connected:
			print("Connected!")
			UI.buttonLib[0].pressed.connect(toggleActivation)
			connected = true
	if !is_instance_valid(UI) || !UI:
		connected = false


func burn_fuel():
	var furnace_inventory = world_inv.get_inv_slots(blockID)
	if furnace_inventory[2].amount > 0 && $BurnTimer.time_left == 0 && "burnable" in furnace_inventory[2].item.attributes:
		var burntime =furnace_inventory[2].item.defValues["burntime"]
		print("burning item!")
		#change this to actually change
		burning = true
		world_inv.removeItem(blockID, 2)
		$BurnTimer.wait_time = burntime
		$BurnTimer.start()

func _on_burn_timer_timeout() -> void:
	print("timeout!")
	burning = false
