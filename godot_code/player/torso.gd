extends Sprite2D


# Called when the node enters the scene tree for the first time.
func _ready():
	pass  #set_as_top_level(true)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#print(position, " ", Global.player.position)
	position += (Vector2(0, -15) - position) / 5
	#position.y += 20
