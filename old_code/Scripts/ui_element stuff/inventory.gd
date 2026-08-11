extends Resource

class_name Inv

signal update

@export var slots: Array[InvSlot]

func editInventory(item: InvItem, place):
	if slots[place].item && item && slots[place].item.name == item.name:
		slots[place].amount += item.amount
	else:
		slots[place].item = item
		if item:
			slots[place].amount = item.amount
		else:
			slots[place].amount = 0
	update.emit()

func removeFromInventory(place):
	slots[place].amount -= 1
	if slots[place].amount <= 0:
		slots[place].item = null
	update.emit()

func removeByItem(item):
	var pslotindex = []
	for _i in slots:
		if _i.item && _i.item.name == item.name:
			pslotindex.append(slots.find(_i))
	var min: int = 128
	var minI: int = -1
	for _i in pslotindex:
		if slots[_i].amount <= min:
			min = slots[_i].amount
			minI = _i
	removeFromInventory(minI)

func appendInventory(item, amount):
	var id: int = -1
	var switchup: bool = false
	for _i in slots:
		if _i.item && _i.item.name == item.name && (amount + _i.amount) < _i.capacity:
			id = slots.find(_i)
			break
		if _i.item && _i.item.name == item.name && (amount + _i.amount) >= _i.capacity && _i.amount != _i.capacity:
			switchup = true
			id = slots.find(_i)
			break
	for _i in slots:
		if id == -1 && !_i.item:
			id = slots.find(_i)
			break
	slots[id].item = item
	slots[id].amount += amount
	if switchup:
		var diff = slots[id].amount - slots[id].capacity
		var itemHold = slots[id].item
		slots[id].amount -= diff
		for _i in slots:
			if !_i.item:
				id = slots.find(_i)
				break
		if diff != 0:
			slots[id].amount = diff
			slots[id].item = itemHold
	update.emit()

func getInventorySize():
	return slots.size()
