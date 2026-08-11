extends Resource

class_name InvHolder

signal updateInv

@export var Invent: Array[InvObject]

func editInventory(item: InvItem, place: int, slot: int):
	if Invent[place].slots[slot].item && item && Invent[place].slots[slot].item.name == item.name:
		Invent[place].slots[slot].amount += item.amount
	else:
		Invent[place].slots[slot].item = item
		if item:
			Invent[place].slots[slot].amount = item.amount
		else:
			Invent[place].slots[slot].amount = 0
	updateInv.emit()

func init_inv(inv: Inv, place):
	if Invent.size()-1 < place:
		append_worldInv()
	#print("Size: ", Invent.size(), " Place: ", place)
	Invent[place].inv = inv
	Invent[place].size = inv.getInventorySize()
	for i in range(Invent[place].size):
		var nullslot: InvSlot = InvSlot.new()
		nullslot.capacity = inv.slots[i].capacity
		Invent[place].slots.append(nullslot)

func append_worldInv():
	var nullInvObject: InvObject = InvObject.new()
	Invent.append(nullInvObject)

func get_inventory(place):
	return Invent[place].inv

func get_inv_slots(id):
	return Invent[id].slots

func removeItem(id, place):
	Invent[id].slots[place].amount -= 1
	if Invent[id].slots[place].amount <= 0:
		Invent[id].slots[place].item = null
	updateInv.emit()

func add_to_first_empty(inv: Inv):
	pass
