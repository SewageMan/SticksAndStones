extends Control

@onready var inv: Inv = preload("res://objects/Items/player_inventory.tres")
@onready var craft_slots = self.get_child(1).get_children()
@onready var craft_recipies: Recipe_library = preload("res://MenuStuff/crafting_recipies.tres")
var scroll: int = 0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	update_slots()

func update_slots():
	for i in range(min(craft_recipies.slots.size(), craft_slots.size())):
		craft_slots[i].update_recipe(craft_recipies.slots[i + scroll])

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var UI = get_node("/root/World/CanvasLayer/camera2d/Overworld_UI")
	for _i in craft_slots:
		if _i.recipeObject && _i.areaHasBeenEntered && !_i.hasBeenPilled:
			_i.hasBeenPilled = true
			%Needed_items.init(_i.recipeObject, _i.position)
			%Needed_items.visible = true
		if !_i.areaHasBeenEntered && _i.hasBeenPilled:
			_i.hasBeenPilled = false
			%Needed_items.clear()
			%Needed_items.visible = false
	if Input.is_action_just_pressed("Click"):
		for _i in craft_slots:
			if _i.recipeObject && _i.areaHasBeenEntered:
				var cancraft: Array[bool]
				for i in range(_i.recipeObject.Items.size()):
					cancraft.append(can_craft_from_items(_i.recipeObject.Items[i], _i.recipeObject.amount[i]))
				#print(cancraft)
				var temp: bool = false
				if false in cancraft:
					temp = true
				if !temp:
					inv.appendInventory(_i.recipeObject.resultItem, _i.recipeObject.resultAmount)
					for i in range(_i.recipeObject.Items.size()):
						for j in range(_i.recipeObject.amount[i]):
							inv.removeByItem(_i.recipeObject.Items[i])
	if Input.is_action_just_pressed("shift_click"):
		var temp: bool = false
		for _i in craft_slots:
			if _i.recipeObject && _i.areaHasBeenEntered:
				while !temp:
					var cancraft: Array[bool]
					for i in range(_i.recipeObject.Items.size()):
						cancraft.append(can_craft_from_items(_i.recipeObject.Items[i], _i.recipeObject.amount[i]))
					#print(cancraft)
					if false in cancraft:
						temp = true
						break
					inv.appendInventory(_i.recipeObject.resultItem, _i.recipeObject.resultAmount)
					for i in range(_i.recipeObject.Items.size()):
						for j in range(_i.recipeObject.amount[i]):
							inv.removeByItem(_i.recipeObject.Items[i])
	if !UI.is_open:
		$Needed_items.clear()

func can_craft_from_items(item, amount):
	var camount: int = 0
	var craftable: bool = false
	for _i in inv.slots:
		if _i.item && _i.item.name == item.name:
			camount += _i.amount
	if camount > amount:
		craftable = true
	return craftable

func _on_previous_pressed() -> void:
	if scroll > 0:
		scroll -= 1
	$Label.text = str(scroll) + "/" + "128"
	update_slots()


func _on_next_pressed() -> void:
	scroll += 1
	$Label.text = str(scroll) + "/" + "128"
	update_slots()
