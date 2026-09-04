extends CharacterBody2D

class_name Player

# Declare member variables here. Examples:
var maxvelocity = 6.5
@onready var root = get_node("/root/World")
@onready var hand = %Hand_item
@onready var animTree = %AnimationTree
var smoothHand: float = 0
var maxMinedistance: float = 250
var animValue = 2

var event_bus: PlayerEventBus
var allowed_to_move: bool = true

var zoom_strength: float = 0.05
var zoom_out_multiplier: float = 1 - zoom_strength
var zoom_in_multiplier: float = 1 / zoom_out_multiplier


# Called when the node enters the scene tree for the first time.
func _ready():
	Global.player = self
	print(scale)


func on_cpp_ready() -> void:
	event_bus = PlayerEventBus.new(self)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var dir = Vector2.ZERO
	var mousePos = get_global_mouse_position()
	if allowed_to_move:
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

	# notifying the c++ side where we want to move
	# all physics runs on c++ side, we only get sent new position back
	# new position is recieved in player event bus, go look there for it
	event_bus.send_player_speed(maxvelocity * dir)

	if Input.is_action_just_released("zoom_out"):
		event_bus.multiply_camera_zoom(zoom_out_multiplier)
	if Input.is_action_just_released("zoom_in"):
		event_bus.multiply_camera_zoom(zoom_in_multiplier)

	#Mining stuff
	var mAngle = (mousePos - self.position).angle()
	var mDist: float = (mousePos - self.position).length()
	%Interaction_length.rotation = mAngle + PI / 2
	%Interaction_length.scale.y = clampf(mDist + 40, 0, maxMinedistance)

	#!!!!Animations!!!!
	#Walking and direction shit
	var angle
	if dir != Vector2.ZERO:
		angle = dir.angle()
		animValue = floor(4 * (angle + PI) / PI + 0.1 * PI)
	$Torso/head.play("Idle_" + str(animValue))
	if velocity.length() > 100:
		if animValue:
			$tracks.play("walk_" + str(animValue))
			$tracks.speed_scale = velocity.length() / 180
	else:
		if animValue:
			$tracks.play("Idle_" + str(animValue))
	#Hand animations
	if hand.texture:
		smoothHand += (1.0 - smoothHand) / 10.0
		animTree.set("parameters/Add2/add_amount", smoothHand)
	else:
		smoothHand -= (smoothHand - 0.0) / 10.0
		animTree.set("parameters/Add2/add_amount", smoothHand)
	#mining:
	if Input.is_action_pressed("right_click"):
		animTree.set("parameters/Add2 2/add_amount", 1)
		if mousePos.x > self.position.x:
			smoothHand = 0
	else:
		animTree.set("parameters/Add2 2/add_amount", 0)
	animTree.set("parameters/Blend2/blend_amount", clampf((self.position.x - mousePos.x) / 30.0 - 0.5, 0, 1))
	# print(velocity)
