extends Line2D

@export var pointData = {}

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

#func addpoint(pos):
	#addpoint(pos)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if points.size() == 2:
		add_point(get_point_position(1))
		if get_point_position(1).y > get_point_position(0).y && get_point_position(1).x > get_point_position(0).x:
			set_point_position(1, Vector2(get_point_position(1).x, get_point_position(0).y))
		else:
			set_point_position(1, Vector2(get_point_position(0).x, get_point_position(1).y))

func recStuff(type):
	if get_point_position(2).y > get_point_position(0).y && get_point_position(2).x > get_point_position(0).x:
		set_point_position(1, Vector2(get_point_position(2).x, get_point_position(0).y))
	else:
		set_point_position(1, Vector2(get_point_position(0).x, get_point_position(2).y))
