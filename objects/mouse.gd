extends Node2D

@export var inv: Inv = preload("res://objects/Items/player_inventory.tres")
@export var world_inv: InvHolder = preload("res://objects/Chunk stuff/world_inventory.tres")
@export var m_inv: Inv = preload("res://objects/Entities/mouse_inv.tres")
@onready var inv_slots: Array = %Inner_inventory/NinePatchRect/GridContainer.get_children()
@onready var hot_slots: Array = %Hotslots/NinePatchRect/HBoxContainer.get_children()
@onready var mouseSlot = $Control/HBoxContainer/UI_inv_slot
var playerSlots

#this script is responsible for all UI interactions with the players' mouse

var hasItem = false

# Called when the node enters the scene tree for the first time.
func _ready():
	#getting the player slots into one arrae
	Global.mouse = self
	playerSlots = hot_slots
	for i in range(inv_slots.size()):
		playerSlots.append(inv_slots[i])
	#make sure is true, will break mouse if not
	set_as_top_level(true)
	mouseSlot.get_child(0).visible = false
	mouseSlot.get_child(2).visible = false
	mouseSlot.get_child(3).visible = false

#function for setting a slot blank, take note that an item gets held in the last
#slot of a given intentory resource for the duration of the exchange, so every single
#resource must have an innaccesable slot.
func _item_pickup(slots, inventory):
	for i in range(slots.size()):
		var slot = slots[i]
		if slot.areaHasBeenEntered && slot.slotObject.item:
			#sets item in temp slot
			m_inv.editInventory(slot.slotObject.item, 0)
			#sets temp item amount to the slot amount
			m_inv.slots[0].amount = slot.slotObject.amount
			#updates mouse inventory
			mouseSlot.update(m_inv.slots[0])
			inventory.editInventory(null, i)
			hasItem = true

func _get_free_space(slots, pickedSlot, begining, end):
	var id: int = -1
	for i in range(begining, end):
		if slots[i].item && slots[i].item.name == pickedSlot.item.name && (slots[i].amount + pickedSlot.amount) <= slots[i].capacity:
			id = i
			break
	for i in range(begining, end):
		if id == -1 && !slots[i].item:
			id = i
			break
	return id

#this function is a lot more straight-forward, no weird item exchange nonsense. (nevermind that, its gotten a lot more complicated)
func _item_putdown(slots, inventory):
	for i in range(slots.size()):
		var slot = slots[i]
		if slot.areaHasBeenEntered && !slot.slotObject.item || slot.areaHasBeenEntered && slot.slotObject.item && slot.slotObject.item.name == mouseSlot.slotObject.item.name:
			hasItem = false
			if mouseSlot.slotObject.amount + slot.slotObject.amount <= slot.slotObject.capacity:
				for j in range(mouseSlot.slotObject.amount):
					inventory.editInventory(mouseSlot.slotObject.item, i)
				mouseSlot.update(null)
			else:
				var diff = slot.slotObject.capacity - slot.slotObject.amount
				mouseSlot.slotObject.amount -= diff
				for j in range(diff):
					inventory.editInventory(mouseSlot.slotObject.item, i)
				mouseSlot.update(mouseSlot.slotObject)
				hasItem = true
			
		#Item swapping
		if mouseSlot.slotObject && (slot.areaHasBeenEntered && !slot.slotObject.item || slot.areaHasBeenEntered && slot.slotObject.item && slot.slotObject.item.name != mouseSlot.slotObject.item.name):
			var hold = [mouseSlot.slotObject.item, mouseSlot.slotObject.amount]
			#inventory.editInventory(inventory.slots[i].item, inventory.slots.size() - 1)
			m_inv.editInventory(inventory.slots[i].item, 0)
			m_inv.slots[0].amount = inventory.slots[i].amount
			mouseSlot.update(m_inv.slots[0])
			inventory.editInventory(hold[0], i)
			inventory.slots[i].amount = hold[1]

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float):
	Global.mouse = self
	var mousePos = get_global_mouse_position()
	var entity_inv; var inv_id 
	#check for inventory entity, looks up if there is currently an entity on screen
	var UI = get_node_or_null("/root/World/CanvasLayer/camera2d/Overworld_UI/Ent_inventory")
	var Overworld_UI = get_node_or_null("/root/World/CanvasLayer/camera2d/Overworld_UI")
	if UI:
		entity_inv = UI.index
		#TODO: change this later, to be for any type of inventory
		inv_id = get_node("/root/World/CanvasLayer/camera2d/Overworld_UI/Ent_inventory").inventory_id
		
	position = mousePos
	#shiftclicking in and out of slots
	if Input.is_action_just_pressed("shift_click") && !hasItem && Overworld_UI.is_open:
		if entity_inv:
			for i in range(playerSlots.size()):
				var slot = playerSlots[i]
				#print(slot.areaHasBeenEntered)
				if slot.areaHasBeenEntered && slot.slotObject.item:
					var validIndex = _get_free_space(world_inv.Invent[UI.inventory_id].slots, slot.slotObject, 0, world_inv.Invent[UI.inventory_id].slots.size())
					var validSlot = world_inv.Invent[UI.inventory_id].slots[validIndex]
					if validSlot.capacity >= slot.slotObject.amount:
						for j in range(slot.slotObject.amount):
							world_inv.editInventory(slot.slotObject.item, inv_id, validIndex)
						inv.editInventory(null, i)
					else:
						for j in range(validSlot.capacity):
							world_inv.editInventory(slot.slotObject.item, inv_id, validIndex)
						inv.slots[i].amount -= validSlot.capacity
			for i in range(entity_inv.size()):
				var slot = entity_inv[i]
				#print(slot.areaHasBeenEntered)
				if slot.areaHasBeenEntered && slot.slotObject.item:
					var validSlot = _get_free_space(inv.slots, slot.slotObject, 0, 8)
					for j in range(slot.slotObject.amount):
						inv.editInventory(slot.slotObject.item, validSlot)
					world_inv.editInventory(null, inv_id, i)
		else:
			for i in range(playerSlots.size()):
				var slot = playerSlots[i]
				#print(slot.areaHasBeenEntered)
				if slot.areaHasBeenEntered && slot.slotObject.item:
					if i<9:
						#Putting items to inventory
						var validSlot = _get_free_space(inv.slots, slot.slotObject, 9, 72)
						for j in range(slot.slotObject.amount):
							inv.editInventory(slot.slotObject.item, validSlot)
						inv.editInventory(null, i)
					else:
						#Put items from inventory to hotbar
						var validSlot = _get_free_space(inv.slots, slot.slotObject, 0, 9)
						for j in range(slot.slotObject.amount):
							inv.editInventory(slot.slotObject.item, validSlot)
						inv.editInventory(null, i)
	#picking an item from a filled slot/putting into an empty slot
	if Input.is_action_just_pressed("Click"):
		if !hasItem:
			#item picks up from what slot resource, and with which inventory resource
			_item_pickup(playerSlots, inv)
			if entity_inv:
				#samee thing but for the brought up inventory
				for i in range(entity_inv.size()):
					var slot = entity_inv[i]
					if slot.areaHasBeenEntered && slot.slotObject.item:
						#sets item in temp slot
						m_inv.editInventory(slot.slotObject.item, 0)
						#sets temp item amount to the slot amount
						m_inv.slots[0].amount = slot.slotObject.amount
						#updates mouse inventory
						mouseSlot.update(m_inv.slots[0])
						#print(mouseSlot.slotObject.amount)
						world_inv.editInventory(null, UI.inventory_id, i)
						hasItem = true
		else:
			#item put down from what slot resource, and with which inventory resource
			_item_putdown(playerSlots, inv)
			if entity_inv:
				#same but pee pee poo poo
				for i in range(entity_inv.size()):
					var slot = entity_inv[i]
					if slot.areaHasBeenEntered && !slot.slotObject.item || slot.areaHasBeenEntered && slot.slotObject.item && slot.slotObject.item.name == mouseSlot.slotObject.item.name:
						hasItem = false
						if mouseSlot.slotObject.amount + slot.slotObject.amount <= slot.slotObject.capacity:
							print("I do this: ", slot.slotObject.amount, " / ", slot.slotObject.capacity)
							for j in range(mouseSlot.slotObject.amount):
								world_inv.editInventory(mouseSlot.slotObject.item, inv_id, i)
							mouseSlot.update(null)
						else:
							#Crash here too
							var diff = slot.slotObject.capacity - slot.slotObject.amount
							mouseSlot.slotObject.amount -= diff
							for j in range(diff):
								world_inv.editInventory(mouseSlot.slotObject.item, inv_id, i)
							mouseSlot.update(mouseSlot.slotObject)
							hasItem = true
						
	if Input.is_action_just_pressed("right_click"):
		if !hasItem:
			#right click for items in the inventory
			for i in range(playerSlots.size()):
				var slot = playerSlots[i]
				if slot.areaHasBeenEntered && slot.slotObject.item:
					#sets item in temp slot
					m_inv.editInventory(slot.slotObject.item, 0)
					#sets temp item amount to the slot amount
					m_inv.slots[0].amount = floor(slot.slotObject.amount/2)
					#updates mouse inventory
					if m_inv.slots[0].amount != 0:
						mouseSlot.update(m_inv.slots[0])
						for j in range(mouseSlot.slotObject.amount):
							inv.removeFromInventory(i)
						hasItem = true
			#right click for items in the inv entity
			if entity_inv:
				for i in range(entity_inv.size()):
					var slot = entity_inv[i]
					if slot.areaHasBeenEntered && slot.slotObject.item:
						#sets item in temp slot
						m_inv.editInventory(slot.slotObject.item, 0)
						#sets temp item amount to the slot amount
						m_inv.slots[0].amount = floor(slot.slotObject.amount/2)
						#updates mouse inventory
						if m_inv.slots[0].amount != 0:
							mouseSlot.update(m_inv.slots[0])
							for j in range(mouseSlot.slotObject.amount):
								world_inv.removeItem(inv_id, i)
							hasItem = true
		else:
			for i in range(playerSlots.size()):
				var slot = playerSlots[i]
				if slot.areaHasBeenEntered && !slot.slotObject.item || slot.areaHasBeenEntered && slot.slotObject.item && slot.slotObject.item.name == mouseSlot.slotObject.item.name && slot.slotObject.amount != slot.slotObject.capacity:
					inv.editInventory(mouseSlot.slotObject.item, i)
					m_inv.slots[0].amount -= 1
					mouseSlot.update(m_inv.slots[0])
					if mouseSlot.slotObject.amount == 0:
						mouseSlot.update(null)
						hasItem = false
			if entity_inv:
				for i in range(entity_inv.size()):
					var slot = entity_inv[i]
					if slot.areaHasBeenEntered && !slot.slotObject.item || slot.areaHasBeenEntered && slot.slotObject.item && slot.slotObject.item.name == mouseSlot.slotObject.item.name && slot.slotObject.amount != slot.slotObject.capacity:
						world_inv.editInventory(mouseSlot.slotObject.item, inv_id, i)
						m_inv.slots[0].amount -= 1
						mouseSlot.update(m_inv.slots[0])
						if mouseSlot.slotObject.amount == 0:
							mouseSlot.update(null)
							hasItem = false
	

#DO NOT LOOOK DOWN!!!!! THERES NOTHING THERE!

func setvisibility(state):
	self.visible = state

func _on_check_box_mouse_entered() -> void:
	setvisibility(false)


func _on_check_box_mouse_exited() -> void:
	setvisibility(true)

func _on_item_search_mouse_entered() -> void:
	setvisibility(false)


func _on_item_search_mouse_exited() -> void:
	setvisibility(true)


func _on_previous_mouse_entered() -> void:
	setvisibility(false)


func _on_previous_mouse_exited() -> void:
	setvisibility(true)


func _on_next_mouse_entered() -> void:
	setvisibility(false)


func _on_next_mouse_exited() -> void:
	setvisibility(true)
