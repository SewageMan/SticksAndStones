extends Control

@export var inv: Inv = preload("res://objects/Items/player_inventory.tres")
@export var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@onready var inventory_ui = preload("res://MenuStuff/inventory_ui's.tscn")
@onready var info: InvResource = preload("res://Scripts/worldResourceLogic/ResourceLib.tres")
@onready var circuit_ui = preload("res://MenuStuff/Circuit/circuit_menu.tscn")
#debug:
@onready var item1 = preload("res://objects/Items/Stick_collectable.tres")
@onready var item2 = preload("res://objects/Items/Stone_collectable.tres")
@onready var item3 = preload("res://objects/Items/chest_placeable.tres")
@onready var item4 = preload("res://objects/Items/stone_furnace_placeable.tres")
@onready var item5 = preload("res://objects/Electrical/Electric_cable.tres")
@onready var item6 = preload("res://objects/Electrical/Resistor_placeable.tres")
@onready var item7 = preload("res://objects/Electrical/Ground_placeable.tres")
@onready var item8 = preload("res://objects/Electrical/Battery_placeable.tres")
#-----
@onready var selector = $Hotslot_selector
@onready var inv_slots: Array = $Inner_inventory/NinePatchRect/GridContainer.get_children()
@onready var hot_slots: Array = $Hotslots/NinePatchRect/HBoxContainer.get_children()
#@onready var chest_slots: Array = $chest_inventory/NinePatchRect/GridContainer.get_children()
@onready var inv_slot_amount: Array
var is_open = false
var test = false
@export var test2 = 0
var selected_slot: int
var invSlots

func _ready():
	inv.appendInventory(item1, 125)
	inv.appendInventory(item2, 37)
	inv.appendInventory(item3, 128)
	inv.appendInventory(item3, 20)
	inv.appendInventory(item4, 3)
	inv.appendInventory(item5, 32)
	inv.appendInventory(item6, 3)
	inv.appendInventory(item7, 5)
	inv.appendInventory(item8, 2)
	
	inv.update.connect(update_slots)
	world_inv.updateInv.connect(_update_inv_slots)
	update_slots()
	close()

func _open_circuit(index):
	var circuitMenu = circuit_ui.instantiate()
	circuitMenu.name = "Circuit_menu"
	circuitMenu.position += Global.camera.position
	%Hotslots.visible = false
	$Hotslot_selector.visible = false
	$Intent.visible = false
	#loading in if this index exists
	if index in WorldSave.circuit_data.keys():
		pass
	Global.currentlyOpenUI = circuitMenu
	add_child(circuitMenu)
	is_open = true

func _close_circuit():
	is_open = false
	%Hotslots.visible = true
	$Hotslot_selector.visible = true
	$Intent.visible = true
	if $Circuit_menu:
		$Circuit_menu.queue_free()

#DEBUG:
func _unhandled_key_input(event: InputEvent) -> void:
	#Debug, this func will usually run when compiling the blackbox
	if !is_open:
		if event.pressed && event.keycode == KEY_J:
			_open_circuit(0)

func _open_inventory(id, Name):
	#this shit just plain sucks
	var inventory = inventory_ui.instantiate()
	var names = info.name_index
	var type = names[Name][1]
	inventory.name = "Ent_inventory"
	inventory.inventory_id = id
	add_child(inventory)
	invSlots = inventory.invLib[type]
	inventory.init(type)
	for i in range(min(world_inv.Invent[id].slots.size(), invSlots.size())):
		invSlots[i].update(world_inv.Invent[id].slots[i])
	Global.currentlyOpenUI = inventory
	is_open = true

func _update_inv_slots():
	var inventory = get_node_or_null("/root/World/CanvasLayer/camera2d/Overworld_UI/Ent_inventory")
	if inventory:
		var id = inventory.inventory_id
		for i in range(min(world_inv.Invent[id].slots.size(), invSlots.size())):
			invSlots[i].update(world_inv.Invent[id].slots[i])

#Figure out why the world inventory is not unique (what?)
func _close_inventory():
	is_open = false
	Global.currentlyOpenUI = null
	if $Ent_inventory:
		$Ent_inventory.queue_free()

func update_slots():
	for i in range(min(inv.slots.size() - 9, inv_slots.size())):
		inv_slots[i].update(inv.slots[i+9])
	for i in range(9):
		hot_slots[i].update(inv.slots[i])

func _process(delta: float):
	if Global.handItem:
		%Item_name.text = Global.handItem.name
	else:
		%Item_name.text = "[Null]"
	#this is a test
	if Input.is_action_just_pressed("Inventory"):
		if !is_open:
			open()
		else:
			_close_circuit()
			_close_inventory()
			close()
	for i in range(9):
		if Input.is_action_just_pressed("Slot_" + str(i+1)):
			selector.position.x = 20 + 36*i
			selected_slot = i
		if hot_slots[selected_slot].slotObject.item:
			Global.handItem = hot_slots[selected_slot].slotObject.item
		else:
			Global.handItem = null

#Opening/closing a chest
func close_chest():
	$chest_inventory.visible = false
func open_chest():
	$chest_inventory.visible = true

#Openning/closing inner inventory
func close():
	$Inner_inventory.visible = false
	var invSlots = $Inner_inventory.get_child(0).get_child(0).get_children()
	var craftSlots = $Inner_inventory.get_child(1).get_child(1).get_children()
	for _i in invSlots:
		_i.visible = false
		_i.areaHasBeenEntered = false
	for _i in craftSlots:
		_i.visible = false
		_i.areaHasBeenEntered = false
	is_open = false
func open():
	$Inner_inventory.visible = true
	var invSlots = $Inner_inventory.get_child(0).get_child(0).get_children()
	var craftSlots = $Inner_inventory.get_child(1).get_child(1).get_children()
	for _i in invSlots:
		_i.visible = true
	for _i in craftSlots:
		_i.visible = true
	is_open = true
