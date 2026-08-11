extends Area2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	self.collision_layer = 12
	self.collision_mask = 12


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
