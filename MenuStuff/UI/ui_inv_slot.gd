extends Panel

@onready var slotObject: InvSlot
@onready var recipeObject: Recipe
@onready var item_visual: Sprite2D = $Item_sprite
@onready var number_visual: Label = $Item_amount
@onready var flash: ColorRect = $ColorRect
@onready var item_name: String
@onready var item_showname: Label = %Item_name
@onready var panel: Panel = %Panel
@onready var areaHasBeenEntered = false
var hasBeenPilled: bool = false
var isVisible = true

func update(slot: InvSlot):
	slotObject = slot
	if slot:
		if !slot.item:
			isVisible = false
			item_visual.visible = false
			number_visual.visible = false
		else:
			isVisible = true
			item_visual.visible = true
			number_visual.visible = true
			number_visual.text = str(slot.amount)
			item_visual.texture = slot.item.texture
			item_name = slot.item.name
			#print("ItemGotten succesfully")
	else:
		isVisible = false
		item_visual.visible = false
		number_visual.visible = false

func update_recipe(recipe: Recipe):
	recipeObject = recipe
	if recipe:
		if !recipe.resultItem:
			isVisible = false
			item_visual.visible = false
			number_visual.visible = false
		else:
			isVisible = true
			item_visual.visible = true
			number_visual.visible = true
			number_visual.text = str(recipe.resultAmount)
			item_visual.texture = recipe.resultItem.texture
	else:
		isVisible = false
		item_visual.visible = false
		number_visual.visible = false

func _on_area_2d_body_entered(body: Node2D) -> void:
	if self.visible:
		areaHasBeenEntered = true
		flash.self_modulate = Color(1,1,1,0.1)
		panel.z_index = 30
		item_showname.z_index = 30
		if slotObject && slotObject.item:
			item_showname.text = slotObject.item.name
			panel.visible = true
		else:
			item_showname.text = ""
		item_showname.visible = true
		#print(self.name)
		#print(body.name)
		#print("ENTERED")
	else:
		areaHasBeenEntered = false

func _on_area_2d_body_exited(body: Node2D) -> void:
	panel.visible = false
	item_showname.visible = false
	if self.visible:
		areaHasBeenEntered = false
		flash.self_modulate = Color(1,1,1,0)
		#print(self.name)
		#print("EXITED")
