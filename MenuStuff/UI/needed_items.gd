extends HBoxContainer

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass
	#set_as_top_level(true)

func init(recipe: Recipe, slotPosition):
	self.position = slotPosition - Vector2(16*(recipe.Items.size()-1),5)
	var slotObject = preload("res://MenuStuff/UI/ui_inv_slot.tscn")
	for i in range(recipe.Items.size()):
		var slot = slotObject.instantiate()
		var Nullslot: InvSlot = InvSlot.new()
		Nullslot.item = recipe.Items[i]
		Nullslot.amount = recipe.amount[i]
		add_child(slot)
		slot.z_index = 30
		slot.update(Nullslot)

func clear():
	for _i in self.get_children():
		_i.queue_free()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
