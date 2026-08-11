extends CharacterBody2D


# Declare member variables here. Examples:
var maxvelocity = Vector2(180, 180) 
@onready var root = get_node("/root/World")
@onready var hand = %Hand_item
@onready var animTree = %AnimationTree
@onready var inv: Inv = preload("res://objects/Items/player_inventory.tres")
@onready var item1 = preload("res://objects/Items/Stick_collectable.tres")
@onready var item2 = preload("res://objects/Items/Stone_collectable.tres")
@onready var itemdrops = [item1, null, item2]
@onready var itemdropAmount = [3, null, 5]
var smoothHand: float = 0
var maxMinedistance: float = 250
var touchingEntities: Array
var animValue = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	Global.player = self

			
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var dir = Vector2.ZERO
	var UI = Global.camera.get_children()[1]
	var progressBar = UI.get_child(0).get_child(2)
	var mouseEntity = UI.get_child(3)
	var mousePos = get_global_mouse_position()
	if Global.handItem:
		hand.texture = Global.handItem.texture
	else:
		hand.texture = null
	if !UI.is_open:
		Global.movementFlag = true
	else:
		Global.movementFlag = false
	if Global.movementFlag:
		if Input.is_action_pressed("move_up"):
			dir.y -= 1
		if Input.is_action_pressed("move_down"):
			dir.y += 1
		if Input.is_action_pressed("move_left"):
			dir.x -= 1
		if Input.is_action_pressed("move_right"):
			dir.x += 1
	#movement inputs
	dir = dir.normalized()
	if Input.is_action_pressed("sprint"):
		dir *= 1.5
	velocity += (maxvelocity * dir - velocity)/5 
	move_and_slide()
	#Mining stuff
	var mAngle = (mousePos - self.position).angle()
	var mDist: float = (mousePos - self.position).length()
	%Interaction_length.rotation = mAngle + PI/2
	%Interaction_length.scale.y = clampf(mDist + 40, 0, maxMinedistance)
	if Input.is_action_pressed("right_click") && touchingEntities.size() > 0 && !mouseEntity.hasItem && !UI.is_open:
		progressBar.visible = true
		touchingEntities[0].mine -= 0.1
		progressBar.scale.x = touchingEntities[0].mine/15.0
		if touchingEntities[0].mine <= 0:
			var world_nodes = get_node_or_null("/root/World").get_children()
			var id = touchingEntities[0].ID
			for _i in world_nodes:
				if "Chunk" in _i.name:
					var index = _i.chunk_data[2][id].find(touchingEntities[0].position)
					if index != -1:
						_i.chunk_data[2][id].remove_at(index)
						_i.chunk_data[1][id] -= 1
			inv.appendInventory(itemdrops[id], itemdropAmount[id])
			touchingEntities[0].queue_free()
	#!!!!Animations!!!!
	#Walking and direction shit
	var angle
	if dir != Vector2.ZERO:
		angle = dir.angle()
		animValue = floor(4*(angle+PI)/PI + 0.1*PI)
	$Torso/head.play("Idle_" + str(animValue))
	if velocity.length() > 100:
		if animValue:
			$tracks.play("walk_" + str(animValue))
			$tracks.speed_scale = velocity.length()/180
	else:
		if animValue:
			$tracks.play("Idle_" + str(animValue))
	#Hand animations
	if hand.texture:
		smoothHand += (1.0 - smoothHand)/10.0
		animTree.set("parameters/Add2/add_amount", smoothHand)
	else:
		smoothHand -= (smoothHand - 0.0)/10.0
		animTree.set("parameters/Add2/add_amount", smoothHand)
	#mining:
	if Input.is_action_pressed("right_click") && !mouseEntity.hasItem && !UI.is_open:
		animTree.set("parameters/Add2 2/add_amount", 1)
		if mousePos.x > self.position.x:
			smoothHand = 0
	else:
		animTree.set("parameters/Add2 2/add_amount", 0)
	animTree.set("parameters/Blend2/blend_amount", clampf((self.position.x - mousePos.x)/30.0 - 0.5, 0, 1))
	# print(velocity)


func _on_interaction_length_body_entered(body: Node2D) -> void:
	if "static_entity" in body.name:
		touchingEntities.append(body)
	touchingEntities = bubbleSortByPosition(touchingEntities)

func bubbleSortByPosition(entities):
	for n in range(len(entities) - 1, 0, -1):
		var swapped = false  
		for i in range(n):
			var Adist = (entities[i].position - self.position).length()
			var Bdist = (entities[i+1].position - self.position).length()
			if Adist > Bdist:
				var hold = entities[i]
				entities[i] = entities[i + 1]
				entities[i+1] = hold
				swapped = true
			if not swapped:
				break
	return entities

func _on_interaction_length_body_exited(body: Node2D) -> void:
	touchingEntities.erase(body)
