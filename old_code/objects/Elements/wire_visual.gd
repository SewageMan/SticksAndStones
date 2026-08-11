extends Line2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass
	#add_point(Vector2(150,150))
	#add_point(Vector2(150,150))
	#add_point(Vector2(150,150))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var mpos = get_global_mouse_position() - Global.panelPos
	if points.size()>1:
		set_point_position(2, mpos)
		if get_point_position(2).y > get_point_position(0).y && get_point_position(2).x > get_point_position(0).x:
			set_point_position(1, Vector2(get_point_position(2).x, get_point_position(0).y))
		else:
			set_point_position(1, Vector2(get_point_position(0).x, get_point_position(2).y))
	
